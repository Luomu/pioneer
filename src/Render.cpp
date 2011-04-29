#include "Render.h"

static GLuint boundArrayBufferObject = 0;
static GLuint boundElementArrayBufferObject = 0;

namespace Render {

static bool initted = false;
static bool shadersEnabled;
static bool shadersAvailable;
static bool isHDREnabled = false;
Shader *simpleShader;
Shader *planetRingsShader[4];

SHADER_CLASS_BEGIN(PostprocessShader)
	SHADER_UNIFORM_SAMPLER(fboTex)
SHADER_CLASS_END()

SHADER_CLASS_BEGIN(PostprocessComposeShader)
	SHADER_UNIFORM_SAMPLER(fboTex)
	SHADER_UNIFORM_SAMPLER(bloomTex)
	SHADER_UNIFORM_SAMPLER(streakTex)
	SHADER_UNIFORM_FLOAT(avgLum)
	SHADER_UNIFORM_FLOAT(middleGrey)
SHADER_CLASS_END()

SHADER_CLASS_BEGIN(PostprocessDownsampleShader)
	SHADER_UNIFORM_SAMPLER(fboTex)
	SHADER_UNIFORM_FLOAT(avgLum)
	SHADER_UNIFORM_FLOAT(middleGrey)
SHADER_CLASS_END()

SHADER_CLASS_BEGIN(PostprocessStreakShader)
	SHADER_UNIFORM_SAMPLER(fboTex)
	SHADER_UNIFORM_VEC2(direction)
	SHADER_UNIFORM_INT(iteration)
	SHADER_UNIFORM_FLOAT(attenuation)
SHADER_CLASS_END()

SHADER_CLASS_BEGIN(PostprocessGlowShader)
	SHADER_UNIFORM_SAMPLER(Texture0)
	SHADER_UNIFORM_INT(iteration)
	SHADER_UNIFORM_FLOAT(pixelWidth)
	SHADER_UNIFORM_FLOAT(pixelHeight)
SHADER_CLASS_END()

SHADER_CLASS_BEGIN(DownSampleShader)
	SHADER_UNIFORM_FLOAT(scale);
	SHADER_UNIFORM_SAMPLER(Texture0)
SHADER_CLASS_END()

SHADER_CLASS_BEGIN(PresentShader)
	SHADER_UNIFORM_SAMPLER(Texture0)
	SHADER_UNIFORM_SAMPLER(Texture1)
	SHADER_UNIFORM_SAMPLER(Texture2)
	SHADER_UNIFORM_SAMPLER(Texture3)
	SHADER_UNIFORM_SAMPLER(Texture4)
SHADER_CLASS_END()

//combine five different size textures
SHADER_CLASS_BEGIN(GlowCompositeShader)
	SHADER_UNIFORM_SAMPLER(Texture0)
	SHADER_UNIFORM_SAMPLER(Texture1)
	SHADER_UNIFORM_SAMPLER(Texture2)
	SHADER_UNIFORM_SAMPLER(Texture3)
	SHADER_UNIFORM_SAMPLER(Texture4)
SHADER_CLASS_END()

//combine four equal size textures
SHADER_CLASS_BEGIN(StreakCompositeShader)
	SHADER_UNIFORM_SAMPLER(Texture0)
	SHADER_UNIFORM_SAMPLER(Texture1)
	SHADER_UNIFORM_SAMPLER(Texture2)
	SHADER_UNIFORM_SAMPLER(Texture3)
SHADER_CLASS_END()

PostprocessDownsampleShader *postprocessBloom1Downsample;
PostprocessShader *postprocessBloom2Downsample, *postprocessBloom3VBlur, *postprocessBloom4HBlur, *postprocessLuminance;
PostprocessComposeShader *postprocessCompose;
PostprocessStreakShader *postprocessStreak;
PostprocessGlowShader *postprocessGlow;
PresentShader *present;
DownSampleShader *downsample;
GlowCompositeShader *glowComposite;
StreakCompositeShader *streakComposite;

SHADER_CLASS_BEGIN(BillboardShader)
	SHADER_UNIFORM_SAMPLER(some_texture)
SHADER_CLASS_END()

BillboardShader *billboardShader;

int State::m_numLights = 1;
float State::m_znear = 10.0f;
float State::m_zfar = 1e6f;
float State::m_invLogZfarPlus1;
Shader *State::m_currentShader = 0;

void BindArrayBuffer(GLuint bo)
{
	if (boundArrayBufferObject != bo) {
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, bo);
		boundArrayBufferObject = bo;
	}
}

bool IsArrayBufferBound(GLuint bo)
{
	return boundArrayBufferObject == bo;
}

void BindElementArrayBuffer(GLuint bo)
{
	if (boundElementArrayBufferObject != bo) {
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, bo);
		boundElementArrayBufferObject = bo;
	}
}

bool IsElementArrayBufferBound(GLuint bo)
{
	return boundElementArrayBufferObject == bo;
}

void UnbindAllBuffers()
{
	BindElementArrayBuffer(0);
	BindArrayBuffer(0);
}

void ScreenAlignedQuad()
{
	glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1.f, 0.f, 0.f);
		glTexCoord2f(0.f, 0.f);
		glVertex2f(0.f, 0.f);
		glColor3f(0.f, 1.f, 0.f);
		glTexCoord2f(1.f, 0.f);
		glVertex2f(1.f, 0.f);;
		glColor3f(0.f, 0.f, 1.f);
		glTexCoord2f(0.f, 1.f);
		glVertex2f(0.f, 1.f);
		glColor3f(0.f, 1.f, 1.f);
		glTexCoord2f(1.f, 1.f);
		glVertex2f(1.f, 1.f);
	glEnd();
}

namespace Post {
	class Pass
	{
	public:
		Pass() :
			_width(-1),
			_height(-1),
			_ok(false)
		{ }
			
		Pass(const int width, const int height) :
			_width(width),
			_height(height),
			_ok(false)
		{ }
		static void GenerateBufferAndTexture(GLuint *buf, GLuint *tex, const int width, const int height)
		{
			glGenFramebuffersEXT(1, buf);
			glGenTextures(1, tex);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, *buf);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, *tex);
			glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB, width, height, 0, GL_RGB, GL_HALF_FLOAT_ARB, NULL);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, *tex, 0);
			/*if (!CheckFBO()) {
				DeleteBuffers();
				return;
			}*/
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}
		bool IsOk() const { return _ok; }
	protected:
		int _width;
		int _height;
		bool _ok;
	};

	class StreakPass : Pass
	{
	public:
		StreakPass() {}
		~StreakPass() {}
		static void Render(const int width, const int height, GLuint buf, const GLuint sourceTex, const int iteration, float dire[2])
		{
			glViewport(0,0,width, height);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, buf);
			glEnable(GL_TEXTURE_RECTANGLE_ARB);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, sourceTex);
			State::UseProgram(postprocessStreak);
			postprocessStreak->set_fboTex(0);
			postprocessStreak->set_iteration(iteration);
			postprocessStreak->set_attenuation(0.99f);
			postprocessStreak->set_direction(dire);
			ScreenAlignedQuad();
		}
	};

	class DownSamplePass : Pass
	{
	public:
		DownSamplePass()
		{ }
		~DownSamplePass()
		{ }
	};

	class GlowPass : Pass
	{
	public:
		GlowPass() :
			Pass(),
			_iteration(0)
		{ }
		GlowPass(const int width, const int height, const int iteration) :
			Pass(width, height),
			_iteration(iteration)
		{
			//create buffers?	
		}
		~GlowPass() {}
		void Render(GLuint buf, const GLuint sourceTex)
		{
			glViewport(0,0,_width, _height);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, buf);
			glEnable(GL_TEXTURE_RECTANGLE_ARB);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, sourceTex);
			State::UseProgram(postprocessGlow);
			postprocessGlow->set_Texture0(0);
			postprocessGlow->set_iteration(_iteration);
			postprocessGlow->set_pixelWidth(static_cast<float>(_width));
			postprocessGlow->set_pixelHeight(static_cast<float>(_height));
			ScreenAlignedQuad();
		}
	private:
		int _iteration;
	};

	//Combine n textures into one 2D texture
	class CompositePass : public Pass
	{
	public:
		CompositePass() : Pass(-1, -1) {}

		CompositePass(const int width, const int height) :
			Pass(width, height)
		{
			GenBufferAndTexture();	
		}
		~CompositePass()
		{
			//delete buf/tex
			glDeleteTextures(1, &_tex);
			glDeleteFramebuffersEXT(1, &_buf);
		}

		GLuint* texture() { return &_tex; }
	protected:
		void GenBufferAndTexture()
		{
			glGenFramebuffersEXT(1, &_buf);
			glGenTextures(1, &_tex);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _buf);
			glBindTexture(GL_TEXTURE_2D, _tex);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB, _width, _height, 0, GL_RGB, GL_HALF_FLOAT_ARB, NULL);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, _tex, 0);
			//check completeness
			GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
			if (status == GL_FRAMEBUFFER_COMPLETE_EXT)
				_ok = true;
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}
		//virtual void BindTextures() = 0;
		//virtual void SetProgramParameters() = 0;
		virtual void CleanUp() = 0;
		GLuint _tex;
		GLuint _buf;
	};

	class GlowCompositePass : public CompositePass
	{
	public:
		GlowCompositePass()
		{ }

		GlowCompositePass(const int width, const int height) :
			CompositePass(width, height)
		{ }

		~GlowCompositePass()
		{ }

		void Render(GLuint& source1, GLuint& source2, GLuint& source3, GLuint& source4, GLuint& source5)
		{
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _buf);
			BindTextures(source1, source2, source3, source4, source5);
			State::UseProgram(glowComposite);
			glowComposite->set_Texture0(0);
			glowComposite->set_Texture1(1);
			glowComposite->set_Texture2(2);
			glowComposite->set_Texture3(3);
			glowComposite->set_Texture4(4);
			glViewport(0,0,_width,_height);
			ScreenAlignedQuad();
			CleanUp();
			glError();
		}

	protected:
		virtual void BindTextures(GLuint& source1, GLuint& source2,
			GLuint& source3, GLuint& source4, GLuint& source5)
		{
			glEnable(GL_TEXTURE_RECTANGLE_ARB); //0
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, source1);
			glActiveTexture(GL_TEXTURE1);
			glEnable(GL_TEXTURE_RECTANGLE_ARB); //1
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, source2);
			glActiveTexture(GL_TEXTURE2);
			glEnable(GL_TEXTURE_RECTANGLE_ARB);//2
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, source3);
			glActiveTexture(GL_TEXTURE3);
			glEnable(GL_TEXTURE_RECTANGLE_ARB); //3
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, source4);
			glActiveTexture(GL_TEXTURE4);
			glEnable(GL_TEXTURE_RECTANGLE_ARB); //4
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, source5);
		}

		virtual void CleanUp()
		{
			//cleanup
			glDisable(GL_TEXTURE_RECTANGLE_ARB); //4
			glActiveTexture(GL_TEXTURE3);
			glDisable(GL_TEXTURE_RECTANGLE_ARB); //3
			glActiveTexture(GL_TEXTURE2);
			glDisable(GL_TEXTURE_RECTANGLE_ARB); //2
			glActiveTexture(GL_TEXTURE1);
			glDisable(GL_TEXTURE_RECTANGLE_ARB); //1
			glActiveTexture(GL_TEXTURE0);
			glDisable(GL_TEXTURE_RECTANGLE_ARB); //0
		}
	};

	//like glowcomposite, except 4 textures and not scaling
	class StreakCompositePass : public CompositePass
	{
	public:
		StreakCompositePass(const int width, const int height) :
			CompositePass(width, height)
		{ }

		void Render(GLuint& source1, GLuint& source2, GLuint& source3, GLuint& source4)
		{
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _buf);
			BindTextures(source1, source2, source3, source4);
			State::UseProgram(streakComposite);
			streakComposite->set_Texture0(0);
			streakComposite->set_Texture1(1);
			streakComposite->set_Texture2(2);
			streakComposite->set_Texture3(3);
			glViewport(0,0,_width,_height);
			ScreenAlignedQuad();
			CleanUp();
			glError();
		}
	protected:
		void BindTextures(GLuint& source1, GLuint& source2,
			GLuint& source3, GLuint& source4)
		{
			glEnable(GL_TEXTURE_RECTANGLE_ARB); //0
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, source1);
			glActiveTexture(GL_TEXTURE1);
			glEnable(GL_TEXTURE_RECTANGLE_ARB); //1
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, source2);
			glActiveTexture(GL_TEXTURE2);
			glEnable(GL_TEXTURE_RECTANGLE_ARB);//2
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, source3);
			glActiveTexture(GL_TEXTURE3);
			glEnable(GL_TEXTURE_RECTANGLE_ARB); //3
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, source4);
		}

		void CleanUp()
		{
			glDisable(GL_TEXTURE_RECTANGLE_ARB); //3
			glActiveTexture(GL_TEXTURE2);
			glDisable(GL_TEXTURE_RECTANGLE_ARB); //2
			glActiveTexture(GL_TEXTURE1);
			glDisable(GL_TEXTURE_RECTANGLE_ARB); //1
			glActiveTexture(GL_TEXTURE0);
			glDisable(GL_TEXTURE_RECTANGLE_ARB); //0
		}
	};
}

static struct postprocessBuffers_t {
	GLuint fb, halfsizeFb, bloomFb1, bloomFb2, tex, halfsizeTex, bloomTex1, bloomTex2, depthbuffer, luminanceFb, luminanceTex;
	GLuint streakBuf1, streakBuf2, streakBuf3, streakBuf4, streakBuf5, streakTex1, streakTex2, streakTex3, streakTex4, streakTex5;
	GLuint glowBuf1, glowTex1, glowBuf2, glowTex2, glowBuf3, glowTex3, glowBuf4, glowTex4, glowBuf5, glowTex5;
	GLuint quarterBuf, quarterTex;
	GLuint eighthBuf, eighthTex;
	GLuint sixteenthBuf, sixteenthTex;
	GLuint thirtytwoBuf, thirtytwoTex;
	GLuint sixtyfourthBuf, sixtyfourthTex;
	int width, height;
	Post::GlowPass glowPass1;
	Post::GlowPass glowPass2;
	Post::GlowPass glowPass3;
	Post::GlowPass glowPass4;
	Post::GlowPass glowPass5;
	Post::GlowCompositePass* glowCompositePass;
	Post::StreakCompositePass* streakCompositePass;
	postprocessBuffers_t() {
		memset(this, 0, sizeof(postprocessBuffers_t));
	}
	bool CheckFBO()
	{
		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
			fprintf(stderr, "Hm. Framebuffer status 0x%x. No HDR for you pal.\n", (int)status);
			return false;
		} else {
			return true;
		}
	}
	void CreateBuffers(int screen_width, int screen_height) {
		width = screen_width;
		height = screen_height;
		glGenFramebuffersEXT(1, &halfsizeFb);
		glGenTextures(1, &halfsizeTex);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, halfsizeFb);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, halfsizeTex);
		glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB, width>>1, height>>1, 0, GL_RGB, GL_HALF_FLOAT_ARB, NULL);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, halfsizeTex, 0);
		if (!CheckFBO()) {
			DeleteBuffers();
			return;
		}
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		glGenFramebuffersEXT(1, &luminanceFb);
		glGenTextures(1, &luminanceTex);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, luminanceFb);
		glBindTexture(GL_TEXTURE_2D, luminanceTex);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB, 128, 128, 0, GL_RGB, GL_FLOAT, NULL);
		glGenerateMipmapEXT(GL_TEXTURE_2D);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, luminanceTex, 0);
		if (!CheckFBO()) {
			DeleteBuffers();
			return;
		}
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glError();

		glGenFramebuffersEXT(1, &bloomFb1);
		glGenTextures(1, &bloomTex1);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, bloomFb1);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, bloomTex1);
		glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB, width>>2, height>>2, 0, GL_RGB, GL_HALF_FLOAT_ARB, NULL);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, bloomTex1, 0);
		if (!CheckFBO()) {
			DeleteBuffers();
			return;
		}
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		glGenFramebuffersEXT(1, &bloomFb2);
		glGenTextures(1, &bloomTex2);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, bloomFb2);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, bloomTex2);
		glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB, width>>2, height>>2, 0, GL_RGB, GL_HALF_FLOAT_ARB, NULL);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, bloomTex2, 0);
		if (!CheckFBO()) {
			DeleteBuffers();
			return;
		}
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		//downsample
		Post::Pass::GenerateBufferAndTexture(&quarterBuf, &quarterTex, width/4, height/4);
		Post::Pass::GenerateBufferAndTexture(&eighthBuf, &eighthTex, width/8, height/8);
		Post::Pass::GenerateBufferAndTexture(&sixteenthBuf, &sixteenthTex, width/16, height/16);
		Post::Pass::GenerateBufferAndTexture(&thirtytwoBuf, &thirtytwoTex, width/32, height/32);
		Post::Pass::GenerateBufferAndTexture(&sixtyfourthBuf, &sixtyfourthTex, width/64, height/64);

		//streakbufs
		Post::Pass::GenerateBufferAndTexture(&streakBuf1, &streakTex1, width>>2, height>>2);
		Post::Pass::GenerateBufferAndTexture(&streakBuf2, &streakTex2, width>>2, height>>2);
		Post::Pass::GenerateBufferAndTexture(&streakBuf3, &streakTex3, width>>2, height>>2);
		Post::Pass::GenerateBufferAndTexture(&streakBuf4, &streakTex4, width>>2, height>>2);
		Post::Pass::GenerateBufferAndTexture(&streakBuf5, &streakTex5, width>>2, height>>2);
		//end streakbufs

		//glowbufs
		glowPass1 = Post::GlowPass(width/4, height/4, 1);
		Post::Pass::GenerateBufferAndTexture(&glowBuf1, &glowTex1, width/4, height/4);
		glowPass2 = Post::GlowPass(width/8, height/8, 2);
		Post::Pass::GenerateBufferAndTexture(&glowBuf2, &glowTex2, width/8, height/8);
		glowPass3 = Post::GlowPass(width/16, height/16, 3);
		Post::Pass::GenerateBufferAndTexture(&glowBuf3, &glowTex3, width/16, height/16);
		glowPass4 = Post::GlowPass(width/32, height/32, 4);
		Post::Pass::GenerateBufferAndTexture(&glowBuf4, &glowTex4, width/32, height/32);
		glowPass5 = Post::GlowPass(width/64, height/64, 5);
		Post::Pass::GenerateBufferAndTexture(&glowBuf5, &glowTex5, width/64, height/64);
		//end glowbufs

		glGenFramebuffersEXT(1, &fb);
		glGenTextures(1, &tex);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
		glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB, width, height, 0, GL_RGB, GL_HALF_FLOAT_ARB, NULL);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, tex, 0);
		glError();
		
		glGenRenderbuffersEXT(1, &depthbuffer);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthbuffer);
		glError();
		if (!CheckFBO()) {
			DeleteBuffers();
			return;
		}

		glowCompositePass = new Post::GlowCompositePass(width/4,height/4);
		streakCompositePass = new Post::StreakCompositePass(width/4,height/4);

		if(!glowCompositePass->IsOk()) {
			DeleteBuffers();
			return;
		}
		if(!streakCompositePass->IsOk()) {
			DeleteBuffers();
			return;
		}

		postprocessBloom1Downsample = new PostprocessDownsampleShader("postprocessBloom1Downsample");
		postprocessBloom2Downsample = new PostprocessShader("postprocessBloom2Downsample");
		postprocessBloom3VBlur = new PostprocessShader("postprocessBloom3VBlur");
		postprocessBloom4HBlur = new PostprocessShader("postprocessBloom4HBlur");
		postprocessCompose = new PostprocessComposeShader("postprocessCompose");
		postprocessLuminance = new PostprocessShader("postprocessLuminance");
		postprocessStreak = new PostprocessStreakShader("postprocessStreak");
		postprocessGlow = new PostprocessGlowShader("postprocessBlur");
		present = new PresentShader("present");
		downsample = new DownSampleShader("downsample");
		glowComposite = new GlowCompositeShader("glowComposite");
		streakComposite = new StreakCompositeShader("streakComposite");

		glError();
	}
	void DeleteBuffers() {
		if (fb) glDeleteFramebuffersEXT(1, &fb);
		if (halfsizeFb) glDeleteFramebuffersEXT(1, &halfsizeFb);
		if (bloomFb1) glDeleteFramebuffersEXT(1, &bloomFb1);
		if (bloomFb2) glDeleteFramebuffersEXT(1, &bloomFb2);
		if (luminanceFb) glDeleteFramebuffersEXT(1, &luminanceFb);
		if (streakBuf1) glDeleteFramebuffersEXT(1, &streakBuf1);
		if (streakBuf2) glDeleteFramebuffersEXT(1, &streakBuf2);
		if (streakBuf3) glDeleteFramebuffersEXT(1, &streakBuf3);
		if (streakBuf4) glDeleteFramebuffersEXT(1, &streakBuf4);
		if (streakBuf5) glDeleteFramebuffersEXT(1, &streakBuf5);
		fb = halfsizeFb = bloomFb1 = bloomFb2 = luminanceFb = 0;
		streakBuf1 = streakBuf2 = streakBuf3 = streakBuf4 = streakBuf5 = 0;

		if (glowBuf1) glDeleteFramebuffersEXT(1, &glowBuf1);
		if (glowBuf2) glDeleteFramebuffersEXT(1, &glowBuf2);
		if (glowBuf3) glDeleteFramebuffersEXT(1, &glowBuf3);
		if (glowBuf4) glDeleteFramebuffersEXT(1, &glowBuf4);
		if (glowBuf5) glDeleteFramebuffersEXT(1, &glowBuf5);
		glowBuf1 = glowBuf2 = glowBuf3 = glowBuf4 = glowBuf5 = 0;

		if (quarterBuf) glDeleteFramebuffersEXT(1, &quarterBuf);
		if (eighthBuf) glDeleteFramebuffersEXT(1, &eighthBuf);
		if (sixteenthBuf) glDeleteFramebuffersEXT(1, &sixteenthBuf);
		if (thirtytwoBuf) glDeleteFramebuffersEXT(1, &thirtytwoBuf);
		if (sixtyfourthBuf) glDeleteFramebuffersEXT(1, &sixtyfourthBuf);
		quarterBuf = eighthBuf = sixteenthBuf = thirtytwoBuf = sixtyfourthBuf = 0;

		delete postprocessStreak;
		delete postprocessGlow;
		delete present;
		delete downsample;
		delete glowCompositePass;
		delete streakCompositePass;
	}

	//render five levels of glow and composite into one texture
	void DoGlow() {
		//blur
		glowPass1.Render(glowBuf1, quarterTex);

		//downsample blur 1/2 (1/8th screen)
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, eighthBuf);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, glowTex1);
		State::UseProgram(downsample);
		downsample->set_scale(2.f);
		downsample->set_Texture0(0);
		glViewport(0,0,width/8,height/8);
		ScreenAlignedQuad();

		//blur the blur
		glowPass2.Render(glowBuf2, eighthTex);

		//downsample blur 1/2 (1/16th screen)
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, sixteenthBuf);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, glowTex2);
		State::UseProgram(downsample);
		downsample->set_scale(2.f);
		downsample->set_Texture0(0);
		glViewport(0,0,width/16,height/16);
		ScreenAlignedQuad();

		//blur the blurred blur
		glowPass3.Render(glowBuf3, sixteenthTex);

		//downsample 1/2 (1/32nd screen)
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, thirtytwoBuf);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, glowTex3);
		State::UseProgram(downsample);
		downsample->set_scale(2.f);
		downsample->set_Texture0(0);
		glViewport(0,0,width/32, height/32);
		ScreenAlignedQuad();

		//blur the doubly blurred blur
		glowPass4.Render(glowBuf4, thirtytwoTex);

		//one more 1/2 DS (1/64th)
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, sixtyfourthBuf);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, thirtytwoTex);
		State::UseProgram(downsample);
		downsample->set_scale(2.f);
		downsample->set_Texture0(0);
		glViewport(0,0,width/64, height/64);
		ScreenAlignedQuad();

		//blur
		glowPass5.Render(glowBuf5, sixtyfourthTex);

		//composite glow to one texture
		glowCompositePass->Render(glowTex1, glowTex2, glowTex3, glowTex4, glowTex5);
	}

	//Render four streaks, three passes each (2 might be enough) and composite into one texture
	void DoStreaks() {
		// left streak, three passes
		float dire[2] = { 0.5f, 0.5f };
		Post::StreakPass::Render(width/4, height/4, streakBuf1, quarterTex, 1, dire);
		Post::StreakPass::Render(width/4, height/4, streakBuf2, streakTex1, 2, dire);
		Post::StreakPass::Render(width/4, height/4, streakBuf1, streakTex2, 3, dire);

		// right streak, three passes
		dire[0] = -0.5f;
		Post::StreakPass::Render(width/4, height/4, streakBuf2, quarterTex, 1, dire);
		Post::StreakPass::Render(width/4, height/4, streakBuf3, streakTex2, 2, dire);
		Post::StreakPass::Render(width/4, height/4, streakBuf2, streakTex3, 3, dire);

		// bottom streak, three passes
		dire[0] = -0.5f;
		dire[1] = -0.5f;
		Post::StreakPass::Render(width/4, height/4, streakBuf3, quarterTex, 1, dire);
		Post::StreakPass::Render(width/4, height/4, streakBuf4, streakTex3, 2, dire);
		Post::StreakPass::Render(width/4, height/4, streakBuf3, streakTex4, 3, dire);

		// top streak, three passes
		dire[0] = 0.5f;
		dire[1] = -0.5f;
		Post::StreakPass::Render(width/4, height/4, streakBuf4, quarterTex, 1, dire);
		Post::StreakPass::Render(width/4, height/4, streakBuf5, streakTex4, 2, dire);
		Post::StreakPass::Render(width/4, height/4, streakBuf4, streakTex5, 3, dire);

		//composite streaks to one texture
		streakCompositePass->Render(streakTex1, streakTex2, streakTex3, streakTex4); //HACK
	}

#if 1
	void DoPostprocess() {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glDisable(GL_LIGHTING);

		//downsample 1/4
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, quarterBuf);
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
		State::UseProgram(downsample);
		downsample->set_scale(4.f);
		downsample->set_Texture0(0);
		glViewport(0,0,width/4,height/4);
		ScreenAlignedQuad();

		DoGlow();
		DoStreaks();

		//present
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, *(glowCompositePass->texture()));
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, *(streakCompositePass->texture()));
		State::UseProgram(present);
		present->set_Texture0(0);
		present->set_Texture1(1);
		present->set_Texture2(2);
		glViewport(0,0,width,height);
		ScreenAlignedQuad();
		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_RECTANGLE_ARB);

		//cleanup
		State::UseProgram(0);
		glEnable(GL_BLEND);
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glError();
	}
#else
	void DoPostprocess() {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0,1.0,0.0,1.0,-1.0,1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glDisable(GL_LIGHTING);

		// So, to do proper tone mapping of HDR to LDR we need to know the average luminance
		// of the scene. We do this by rendering the scene's luminance to a smaller texture,
		// generating mipmaps for it, and grabbing the luminance at the smallest mipmap level
		glViewport(0,0,128,128);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, luminanceFb);
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
		State::UseProgram(postprocessLuminance);
		postprocessLuminance->set_fboTex(0);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.0, 0.0);
			glVertex2f(0.0, 0.0);
			glTexCoord2f(width, 0.0);
			glVertex2f(1.0, 0.0);
			glTexCoord2f(0.0,height);
			glVertex2f(0.0, 1.0);
			glTexCoord2f(width, height);
			glVertex2f(1.0, 1.0);
		glEnd();
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glDisable(GL_TEXTURE_RECTANGLE_ARB);
		glBindTexture(GL_TEXTURE_2D, luminanceTex);
		glEnable(GL_TEXTURE_2D);
		glGenerateMipmapEXT(GL_TEXTURE_2D);
		float avgLum[4];
		glGetTexImage(GL_TEXTURE_2D, 7, GL_RGB, GL_FLOAT, avgLum);

		//printf("%f -> ", avgLum[0]);
		avgLum[0] = std::max((float)exp(avgLum[0]), 0.03f);
		//printf("%f\n", avgLum[0]);
		// see reinhard algo
		const float midGrey = 1.03f - 2.0f/(2.0f+log10(avgLum[0] + 1.0f));

		//downsample 1/4
		/*glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, quarterBuf);
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
		State::UseProgram(downsample);
		downsample->set_scale(4.f);
		downsample->set_Texture0(0);
		glViewport(0,0,width/4,height/4);
		ScreenAlignedQuad();*/
		//downsample 1/4
		//use the original downsample with the bright pass, except changed scale factor
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, quarterBuf);
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
		State::UseProgram(postprocessBloom1Downsample);
		postprocessBloom1Downsample->set_avgLum(avgLum[0]);
		postprocessBloom1Downsample->set_middleGrey(midGrey);
		postprocessBloom1Downsample->set_fboTex(0);
		glViewport(0,0,width/4,height/4);
		ScreenAlignedQuad();

		DoGlow();

		// left streak, three passes
		float dire[2] = { 0.5f, 0.5f };
		Post::StreakPass::Render(width/4, height/4, streakBuf1, quarterTex, 1, dire);
		Post::StreakPass::Render(width/4, height/4, streakBuf2, streakTex1, 2, dire);
		Post::StreakPass::Render(width/4, height/4, streakBuf1, streakTex2, 3, dire);

		// right streak, three passes
		dire[0] = -0.5f;
		Post::StreakPass::Render(width/4, height/4, streakBuf2, quarterTex, 1, dire);
		Post::StreakPass::Render(width/4, height/4, streakBuf3, streakTex2, 2, dire);
		Post::StreakPass::Render(width/4, height/4, streakBuf2, streakTex3, 3, dire);

		// bottom streak, three passes
		dire[0] = -0.5f;
		dire[1] = -0.5f;
		Post::StreakPass::Render(width/4, height/4, streakBuf3, quarterTex, 1, dire);
		Post::StreakPass::Render(width/4, height/4, streakBuf4, streakTex3, 2, dire);
		Post::StreakPass::Render(width/4, height/4, streakBuf3, streakTex4, 3, dire);

		// top streak, three passes
		dire[0] = 0.5f;
		dire[1] = -0.5f;
		Post::StreakPass::Render(width/4, height/4, streakBuf4, quarterTex, 1, dire);
		Post::StreakPass::Render(width/4, height/4, streakBuf5, streakTex4, 2, dire);
		Post::StreakPass::Render(width/4, height/4, streakBuf4, streakTex5, 3, dire);

		//composite streaks to one texture
		streakCompositePass->Render(streakTex1, streakTex2, streakTex3, streakTex4, streakTex4); //HACK
/*		
		glDisable(GL_TEXTURE_2D);
		glViewport(0,0,width>>1,height>>1);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, halfsizeFb);
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
		State::UseProgram(postprocessBloom1Downsample);
		postprocessBloom1Downsample->set_avgLum(avgLum[0]);
		postprocessBloom1Downsample->set_middleGrey(midGrey);
		postprocessBloom1Downsample->set_fboTex(0);
		ScreenAlignedQuad();

		glViewport(0,0,width>>2,height>>2);
		State::UseProgram(postprocessBloom2Downsample);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, bloomFb1);
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, halfsizeTex);
		ScreenAlignedQuad();

		glViewport(0,0,width>>2,height>>2);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, bloomFb2);
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, bloomTex1);
		State::UseProgram(postprocessBloom3VBlur);
		postprocessBloom3VBlur->set_fboTex(0);
		ScreenAlignedQuad();

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, bloomFb1);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, bloomTex2);
		State::UseProgram(postprocessBloom4HBlur);
		postprocessBloom4HBlur->set_fboTex(0);
		ScreenAlignedQuad();
*/		
		//compose
		glViewport(0,0,width,height);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, *(glowCompositePass->texture()));

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, *(streakCompositePass->texture()));

		State::UseProgram(postprocessCompose);
		postprocessCompose->set_fboTex(0);
		postprocessCompose->set_bloomTex(1);
		postprocessCompose->set_streakTex(2);
		postprocessCompose->set_avgLum(avgLum[0]);
		//printf("Mid grey %f\n", midGrey);
		postprocessCompose->set_middleGrey(midGrey);
		ScreenAlignedQuad();

		//clean up
		State::UseProgram(0);
		//glDisable(GL_TEXTURE_RECTANGLE_ARB);
		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_RECTANGLE_ARB);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glEnable(GL_LIGHTING);

		glError();
	}
#endif
	bool Initted() { return fb ? true : false; }
} s_hdrBufs;

void Init(int screen_width, int screen_height)
{
	if (initted) return;
	shadersAvailable = (GLEW_VERSION_2_0 ? true : false);
	shadersEnabled = shadersAvailable;
	printf("GLSL shaders %s.\n", shadersEnabled ? "on" : "off");
	// Framebuffers for HDR
	if (GLEW_EXT_framebuffer_object && GLEW_ARB_color_buffer_float && GLEW_ARB_texture_rectangle) { // && GLEW_ARB_depth_buffer_float) {
		s_hdrBufs.CreateBuffers(screen_width, screen_height);
	}
	
	initted = true;

	if (shadersEnabled) {
		simpleShader = new Shader("simple");
		billboardShader = new BillboardShader("billboard");
		planetRingsShader[0] = new Shader("planetrings", "#define NUM_LIGHTS 1\n");
		planetRingsShader[1] = new Shader("planetrings", "#define NUM_LIGHTS 2\n");
		planetRingsShader[2] = new Shader("planetrings", "#define NUM_LIGHTS 3\n");
		planetRingsShader[3] = new Shader("planetrings", "#define NUM_LIGHTS 4\n");
	}
}

bool IsHDREnabled() { return (s_hdrBufs.Initted() && isHDREnabled && shadersEnabled) ? 1 : 0; }

bool IsHDRAvailable() { return s_hdrBufs.Initted() ? true : false; }

void PrepareFrame()
{
	if (IsHDREnabled()) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, s_hdrBufs.fb);
	} else {
		if (GLEW_EXT_framebuffer_object) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
}

void PostProcess()
{
	if (IsHDREnabled()) {
		s_hdrBufs.DoPostprocess();
	}
}

void SwapBuffers()
{
	SDL_GL_SwapBuffers();
}

bool AreShadersEnabled()
{
	return shadersEnabled;
}

void ToggleShaders()
{
	if (shadersAvailable) {
		shadersEnabled = (shadersEnabled ? false : true);
	}
	printf("GLSL shaders %s.\n", shadersEnabled ? "on" : "off");
}

void ToggleHDR()
{
	isHDREnabled = !isHDREnabled;
	printf("HDR lighting %s.\n", isHDREnabled ? "enabled" : "disabled");
}

/*
 * So if we are using the z-hack VPROG_POINTSPRITE then this still works.
 */
void PutPointSprites(int num, vector3f *v, float size, const float modulationCol[4], GLuint tex, int stride)
{
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);

//	float quadratic[] =  { 0.0f, 0.0f, 0.00001f };
//	glPointParameterfv( GL_POINT_DISTANCE_ATTENUATION, quadratic );
//	glPointParameterf(GL_POINT_SIZE_MIN, 1.0 );
//	glPointParameterf(GL_POINT_SIZE_MAX, 10000.0 );
		
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glColor4fv(modulationCol);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);	

	// XXX point sprite thing needs some work. remember to enable point
	// sprite shader in LmrModel.cpp
	if (AreShadersEnabled()) {
		// this is a bit dumb since it doesn't care how many lights
		// the scene has, and this is a constant...
		State::UseProgram(billboardShader);
		billboardShader->set_some_texture(0);
	}

//	/*if (Shader::IsVtxProgActive())*/ glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
	if (0) {//GLEW_ARB_point_sprite) {
		glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
		glEnable(GL_POINT_SPRITE_ARB);
		
		glPointSize(size);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, v);
		glDrawArrays(GL_POINTS, 0, num);
		glDisableClientState(GL_VERTEX_ARRAY);
		glPointSize(1);

		glDisable(GL_POINT_SPRITE_ARB);
		glDisable(GL_TEXTURE_2D);
	
	} else {
		// quad billboards
		matrix4x4f rot;
		glGetFloatv(GL_MODELVIEW_MATRIX, &rot[0]);
		rot.ClearToRotOnly();
		rot = rot.InverseOf();

		const float sz = 0.5f*size;
		const vector3f rotv1 = rot * vector3f(sz, sz, 0.0f);
		const vector3f rotv2 = rot * vector3f(sz, -sz, 0.0f);
		const vector3f rotv3 = rot * vector3f(-sz, -sz, 0.0f);
		const vector3f rotv4 = rot * vector3f(-sz, sz, 0.0f);

		glBegin(GL_QUADS);
		for (int i=0; i<num; i++) {
			vector3f pos(*v);
			vector3f vert;

			vert = pos+rotv4;
			glTexCoord2f(0.0f,0.0f);
			glVertex3f(vert.x, vert.y, vert.z);
			
			vert = pos+rotv3;
			glTexCoord2f(0.0f,1.0f);
			glVertex3f(vert.x, vert.y, vert.z);
			
			vert = pos+rotv2;
			glTexCoord2f(1.0f,1.0f);
			glVertex3f(vert.x, vert.y, vert.z);
			
			vert = pos+rotv1;
			glTexCoord2f(1.0f,0.0f);
			glVertex3f(vert.x, vert.y, vert.z);
			
			v = reinterpret_cast<vector3f*>(reinterpret_cast<char*>(v)+stride);
		}
		glEnd();
	}
//	/*if (Shader::IsVtxProgActive())*/ glDisable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

//	quadratic[0] = 1; quadratic[1] = 0;
//	glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );

	glDisable(GL_TEXTURE_2D);
	glDepthMask(GL_TRUE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
}

// -------------- class Shader ----------------


static char *load_file(const char *filename)
{
	FILE *f = fopen(filename, "rb");
	if (!f) {
		//printf("Could not open %s.\n", filename);
		return 0;
	}
	fseek(f, 0, SEEK_END);
	size_t len = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *buf = (char*)malloc(sizeof(char) * (len+1));
	fread(buf, 1, len, f);
	fclose(f);
	buf[len] = 0;
	return buf;
}

static void PrintGLSLCompileError(const char *filename, GLuint obj)
{
	int infologLength = 0;
	char infoLog[1024];

	if (glIsShader(obj))
		glGetShaderInfoLog(obj, 1024, &infologLength, infoLog);
	else
		glGetProgramInfoLog(obj, 1024, &infologLength, infoLog);

	if (infologLength > 0) {
		Warning("Error compiling shader: %s: %s\nOpenGL vendor: %s\nOpenGL renderer string: %s\n\nPioneer will run with shaders disabled.",
				filename, infoLog, glGetString(GL_VENDOR), glGetString(GL_RENDERER));
		shadersAvailable = false;
		shadersEnabled = false;
	}
}
	
bool Shader::Compile(const char *shader_name, const char *additional_defines)
{
	if (!shadersAvailable) {
		m_program = 0;
		return false;
	}
	static char *lib_fs = 0;
	static char *lib_vs = 0;
	static char *lib_all = 0;
	if (!lib_fs) lib_fs = load_file("data/shaders/_library.frag.glsl");
	if (!lib_vs) lib_vs = load_file("data/shaders/_library.vert.glsl");
	if (!lib_all) lib_all = load_file("data/shaders/_library.all.glsl");

	const std::string name = std::string("data/shaders/") + shader_name;
	char *vscode = load_file((name + ".vert.glsl").c_str());
	char *pscode = load_file((name + ".frag.glsl").c_str());
	char *allcode = load_file((name + ".all.glsl").c_str());
	
	if (vscode == 0) {
		Warning("Could not find shader %s.", (name + ".vert.glsl").c_str());
		m_program = 0;
		return false;
	}
		
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	std::vector<const char*> shader_src;

	if (additional_defines) shader_src.push_back(additional_defines);
	shader_src.push_back("#define ZHACK 1\n");
	shader_src.push_back(lib_all);
	shader_src.push_back(lib_vs);
	if (allcode) shader_src.push_back(allcode);
	shader_src.push_back(vscode);

	glShaderSource(vs, shader_src.size(), &shader_src[0], 0);
	glCompileShader(vs);
	GLint status;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
	if (!status) {
		PrintGLSLCompileError((name + ".vert.glsl").c_str(), vs);
		m_program = 0;
		return false;
	}

	GLuint ps = 0;
	if (pscode) {
		shader_src.clear();
		if (additional_defines) shader_src.push_back(additional_defines);
		shader_src.push_back("#define ZHACK 1\n");
		shader_src.push_back(lib_all);
		shader_src.push_back(lib_fs);
		if (allcode) shader_src.push_back(allcode);
		shader_src.push_back(pscode);
		
		ps = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(ps, shader_src.size(), &shader_src[0], 0);
		glCompileShader(ps);
		GLint status;
		glGetShaderiv(ps, GL_COMPILE_STATUS, &status);
		if (!status) {
			PrintGLSLCompileError((name + ".frag.glsl").c_str(), ps);
			m_program = 0;
			return false;
		}
	}

	m_program = glCreateProgram();
	glAttachShader(m_program, vs);
	if (pscode) glAttachShader(m_program, ps);
	glLinkProgram(m_program);
	glGetProgramiv(m_program, GL_LINK_STATUS, &status);
	if (!status) {
		PrintGLSLCompileError(name.c_str(), m_program);
		m_program = 0;
		return false;
	}

	free(vscode);
	if (pscode) free(pscode);
	if (allcode) free(allcode);
	
	return true;
}

// --------------- class Shader ------------------

bool State::UseProgram(Shader *shader)
{
	if (shadersEnabled) {
		if (shader) {
			if (m_currentShader != shader) {
				m_currentShader = shader;
				glUseProgram(shader->GetProgram());
				shader->set_invLogZfarPlus1(m_invLogZfarPlus1);
				return true;
			} else {
				return false;
			}
		} else {
			m_currentShader = 0;
			glUseProgram(0);
			return true;
		}
	} else {
		return false;
	}
}

}; /* namespace Render */
