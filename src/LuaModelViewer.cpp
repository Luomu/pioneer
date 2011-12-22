#include "libs.h"
#include "gui/Gui.h"
#include "collider/collider.h"
#include "LmrModel.h"
#include "ShipType.h"
#include "EquipType.h"
#include "render/Render.h"
#include "Ship.h" // for the flight state and ship animation enums
#include "SpaceStation.h" // for the space station animation enums
#include "ui/UIManager.h"

enum ModelCategory {
	MODEL_OTHER,
	MODEL_SHIP,
	MODEL_SPACESTATION
};

static const char *ANIMATION_NAMESPACES[] = {
	0,
	"ShipAnimation",
	"SpaceStationAnimation",
};

static const int LMR_ARG_MAX = 40;

static SDL_Surface *g_screen;
static int g_width, g_height;
static int g_mouseMotion[2];
static char g_keyState[SDLK_LAST];
static int g_mouseButton[5];
static float g_zbias;
static bool g_doBenchmark = false;

extern void LmrModelCompilerInit();

static int g_wheelMoveDir = -1;
static int g_renderType = 0;
static float g_frameTime;
static EquipSet g_equipment;
static LmrObjParams g_params = {
	0, // animation namespace
	0.0, // time
	{}, // animation stages
	{}, // animation positions
	"PIONEER", // label
	&g_equipment, // equipment
	Ship::FLYING, // flightState

	{ 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 0.0f },

	{	// pColor[3]
	{ { .2f, .2f, .5f, 1 }, { 1, 1, 1 }, { 0, 0, 0 }, 100.0 },
	{ { 0.5f, 0.5f, 0.5f, 1 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
	{ { 0.8f, 0.8f, 0.8f, 1 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } },
};

class Viewer: public Gui::Fixed {
public:
	Gui::Adjustment *m_linthrust[3];
	Gui::Adjustment *m_angthrust[3];
	Gui::Adjustment *m_anim[LMR_ARG_MAX];
	Gui::TextEntry *m_animEntry[LMR_ARG_MAX];
	LmrCollMesh *m_cmesh;
	LmrModel *m_model;
	CollisionSpace *m_space;
	Geom *m_geom;
	ModelCategory m_modelCategory;
	UI::Manager m_uiManager;
	UI::Screen *m_ui;

	void SetModel(LmrModel *);

	void PickModel(const std::string &initial_name, const std::string &initial_errormsg);

	void PickModel() {
		PickModel("", "");
	}

	float GetAnimValue(int i) {
		std::string val = m_animEntry[i]->GetText();
		return float(atof(val.c_str()));
	}

	Viewer(): Gui::Fixed(float(g_width), float(g_height)),
		m_uiManager(g_width, g_height),
		m_quit(false)
        {
		m_model = 0;
		m_cmesh = 0;
		m_geom = 0;
		m_space = new CollisionSpace();
		m_showBoundingRadius = false;
		Gui::Screen::AddBaseWidget(this, 0, 0);
		SetTransparency(true);

		{
			Add(new Gui::Label("Linear thrust"), 0, Gui::Screen::GetHeight()-140.0f);
			for (int i=0; i<3; i++) {
				m_linthrust[i] = new Gui::Adjustment();
				m_linthrust[i]->SetValue(0.5);
				Gui::VScrollBar *v = new Gui::VScrollBar();
				v->SetAdjustment(m_linthrust[i]);
				Add(v, float(i*25), Gui::Screen::GetHeight()-120.0f);
			}
			
			Add(new Gui::Label("Angular thrust"), 100, Gui::Screen::GetHeight()-140.0f);
			for (int i=0; i<3; i++) {
				m_angthrust[i] = new Gui::Adjustment();
				m_angthrust[i]->SetValue(0.5);
				Gui::VScrollBar *v = new Gui::VScrollBar();
				v->SetAdjustment(m_angthrust[i]);
				Add(v, float(100 + i*25), Gui::Screen::GetHeight()-120.0f);
			}
			
			Add(new Gui::Label("Animations (0 gear, 1-4 are time - ignore them comrade)"),
					200, Gui::Screen::GetHeight()-140.0f);
			for (int i=0; i<LMR_ARG_MAX; i++) {
				Gui::Fixed *box = new Gui::Fixed(32.0f, 120.0f);
				Add(box, float(200 + i*25), Gui::Screen::GetHeight()-120.0f);

				m_anim[i] = new Gui::Adjustment();
				m_anim[i]->SetValue(0);
				Gui::VScrollBar *v = new Gui::VScrollBar();
				v->SetAdjustment(m_anim[i]);
				box->Add(v, 0, 42.0f);
				char buf[32];
				snprintf(buf, sizeof(buf), "%d", i);
				box->Add(new Gui::Label(buf), 0, 0);

				m_animEntry[i] = new Gui::TextEntry();
				box->Add(m_animEntry[i], 0, 16.0f);
				m_anim[i]->onValueChanged.connect(sigc::bind(sigc::mem_fun(this, &Viewer::OnAnimChange), m_anim[i], m_animEntry[i]));
				OnAnimChange(m_anim[i], m_animEntry[i]);
			}
		}
		SetupUi();

		ShowAll();
		Show();
	}

	~Viewer();

	void Quit();

	void OnAnimChange(Gui::Adjustment *a, Gui::TextEntry *e) {
		char buf[128];
		snprintf(buf, sizeof(buf), "%.2f", a->GetValue());
		e->SetText(buf);
	}

	void OnResetAdjustments() {
		for (int i=0; i<LMR_ARG_MAX; i++) m_anim[i]->SetValue(0);
		for (int i=0; i<3; i++) {
			m_linthrust[i]->SetValue(0.5);
			m_angthrust[i]->SetValue(0.5);
		}
	}
	void OnClickToggleBenchmark() {
		g_doBenchmark = !g_doBenchmark;
	}
	void OnClickRebuildCollMesh() {
		m_space->RemoveGeom(m_geom);
		delete m_geom;
		delete m_cmesh;

		m_cmesh = new LmrCollMesh(m_model, &g_params);
		m_geom = new Geom(m_cmesh->geomTree);
		m_space->AddGeom(m_geom);
	}

	void OnToggleGearState() {
		if (g_wheelMoveDir == -1) g_wheelMoveDir = +1;
		else g_wheelMoveDir = -1;
	}

	void OnClickChangeView() {
		g_renderType++;
		// XXX raytraced view disabled
		if (g_renderType > 1) g_renderType = 0;
	}

	void OnToggleBoundingRadius() {
		m_showBoundingRadius = !m_showBoundingRadius;
	}

	void MainLoop();
	void SetSbreParams();
private:
	void TryModel(const SDL_keysym *sym, Gui::TextEntry *entry, Gui::Label *errormsg);
	void VisualizeBoundingRadius(matrix4x4f& trans, double radius);
	void PollEvents();
	void SetupUi();
	void OnThrusterUpdate();
	bool m_showBoundingRadius;
	bool m_quit;
};

Viewer::~Viewer()
{
	printf("Viewer quitting.\n");
	if (m_cmesh) delete m_cmesh;
	if (m_geom) delete m_geom;
	delete m_space;
}

void Viewer::Quit()
{
	m_quit = true;
}

void Viewer::SetupUi()
{
	m_ui = m_uiManager.OpenScreen("modelviewer");
	m_ui->GetEventListener("changeview"         )->SetHandler(sigc::mem_fun(this, &Viewer::OnClickChangeView));
	m_ui->GetEventListener("show-boundingradius")->SetHandler(sigc::mem_fun(this, &Viewer::OnToggleBoundingRadius));
	m_ui->GetEventListener("performancetest"    )->SetHandler(sigc::mem_fun(this, &Viewer::OnClickToggleBenchmark));
	m_ui->GetEventListener("resetanimations"    )->SetHandler(sigc::mem_fun(this, &Viewer::OnResetAdjustments));

	//thruster sliders
	m_ui->GetEventListener("thrusterupdate"     )->SetHandler(sigc::mem_fun(this, &Viewer::OnThrusterUpdate));
}

void Viewer::OnThrusterUpdate()
{
}

void Viewer::SetModel(LmrModel *model)
{
	m_model = model;
	// clear old geometry
	if (m_cmesh) delete m_cmesh;
	if (m_geom) {
		m_space->RemoveGeom(m_geom);
		delete m_geom;
	}

	// set up model parameters
	// inefficient (looks up and searches tags table separately for each tag)
	bool has_ship = m_model->HasTag("ship") || m_model->HasTag("static_ship");
	bool has_station = m_model->HasTag("surface_station") || m_model->HasTag("orbital_station");
	if (has_ship && !has_station) {
		m_modelCategory = MODEL_SHIP;
		const std::string name = model->GetName();
		std::map<std::string,ShipType>::const_iterator it = ShipType::types.begin();
		while (it != ShipType::types.end()) {
			if (it->second.lmrModelName == name)
				break;
			else
				++it;
		}
		if (it != ShipType::types.end())
			g_equipment.InitSlotSizes(it->first);
		else
			g_equipment.InitSlotSizes(ShipType::EAGLE_LRF);
		g_params.equipment = &g_equipment;
	} else if (has_station && !has_ship) {
		m_modelCategory = MODEL_SPACESTATION;
		g_params.equipment = 0;
	} else {
		m_modelCategory = MODEL_OTHER;
		g_params.equipment = 0;
	}

	g_params.animationNamespace = ANIMATION_NAMESPACES[m_modelCategory];

	// construct geometry
	m_cmesh = new LmrCollMesh(m_model, &g_params);
	m_geom = new Geom(m_cmesh->geomTree);
	m_space->AddGeom(m_geom);
}

void Viewer::TryModel(const SDL_keysym *sym, Gui::TextEntry *entry, Gui::Label *errormsg)
{
	if (sym->sym == SDLK_RETURN) {
		LmrModel *m = 0;
		try {
			m = LmrLookupModelByName(entry->GetText().c_str());
		} catch (LmrModelNotFoundException) {
			errormsg->SetText("Could not find model: " + entry->GetText());
		}
		if (m) SetModel(m);
	}
}

void Viewer::PickModel(const std::string &initial_name, const std::string &initial_errormsg)
{
	Gui::Fixed *f = new Gui::Fixed();
	f->SetSizeRequest(Gui::Screen::GetWidth()*0.5f, Gui::Screen::GetHeight()*0.5);
	Gui::Screen::AddBaseWidget(f, Gui::Screen::GetWidth()*0.25f, Gui::Screen::GetHeight()*0.25f);

	f->Add(new Gui::Label("Enter the name of the model you want to view:"), 0, 0);

	Gui::Label *errormsg = new Gui::Label(initial_errormsg);
	f->Add(errormsg, 0, 64);

	Gui::TextEntry *entry = new Gui::TextEntry();
	entry->SetText(initial_name);
	entry->onKeyPress.connect(sigc::bind(sigc::mem_fun(this, &Viewer::TryModel), entry, errormsg));
	entry->Show();
	f->Add(entry, 0, 32);

	m_model = 0;

	while (!m_model) {
		this->Hide();
		f->ShowAll();
		PollEvents();
		Render::PrepareFrame();
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Render::PostProcess();
		Gui::Draw();
		glError();
		Render::SwapBuffers();
	}
	Gui::Screen::RemoveBaseWidget(f);
	delete f;
	this->Show();
}

void Viewer::SetSbreParams()
{
	float gameTime = SDL_GetTicks() * 0.001f;

	if (m_modelCategory == MODEL_SHIP) {
		g_params.animValues[Ship::ANIM_WHEEL_STATE] = GetAnimValue(0);

		g_equipment.Set(Equip::SLOT_FUELSCOOP,  0, (GetAnimValue( 5) > 0.5) ? Equip::FUEL_SCOOP            : Equip::NONE);
		g_equipment.Set(Equip::SLOT_ENGINE,     0, (GetAnimValue( 6) > 0.5) ? Equip::DRIVE_CLASS4          : Equip::NONE);
		g_equipment.Set(Equip::SLOT_ECM,        0, (GetAnimValue( 7) > 0.5) ? Equip::ECM_ADVANCED          : Equip::NONE);
		g_equipment.Set(Equip::SLOT_SCANNER,    0, (GetAnimValue( 8) > 0.5) ? Equip::SCANNER               : Equip::NONE);
		g_equipment.Set(Equip::SLOT_ATMOSHIELD, 0, (GetAnimValue( 9) > 0.5) ? Equip::ATMOSPHERIC_SHIELDING : Equip::NONE);
		g_equipment.Set(Equip::SLOT_LASER,      0, (GetAnimValue(10) > 0.5) ? Equip::PULSECANNON_4MW       : Equip::NONE);
		g_equipment.Set(Equip::SLOT_LASER,      1, (GetAnimValue(11) > 0.5) ? Equip::PULSECANNON_4MW       : Equip::NONE);
		g_equipment.Set(Equip::SLOT_MISSILE,    0, (GetAnimValue(12) > 0.5) ? Equip::MISSILE_SMART         : Equip::NONE);
		g_equipment.Set(Equip::SLOT_MISSILE,    1, (GetAnimValue(13) > 0.5) ? Equip::MISSILE_SMART         : Equip::NONE);
		g_equipment.Set(Equip::SLOT_MISSILE,    2, (GetAnimValue(14) > 0.5) ? Equip::MISSILE_SMART         : Equip::NONE);
		g_equipment.Set(Equip::SLOT_MISSILE,    3, (GetAnimValue(15) > 0.5) ? Equip::MISSILE_SMART         : Equip::NONE);
		g_equipment.Set(Equip::SLOT_MISSILE,    4, (GetAnimValue(16) > 0.5) ? Equip::MISSILE_SMART         : Equip::NONE);
		g_equipment.Set(Equip::SLOT_MISSILE,    5, (GetAnimValue(17) > 0.5) ? Equip::MISSILE_SMART         : Equip::NONE);
		g_equipment.Set(Equip::SLOT_MISSILE,    6, (GetAnimValue(18) > 0.5) ? Equip::MISSILE_SMART         : Equip::NONE);
		g_equipment.Set(Equip::SLOT_MISSILE,    7, (GetAnimValue(19) > 0.5) ? Equip::MISSILE_SMART         : Equip::NONE);
	} else if (m_modelCategory == MODEL_SPACESTATION) {
		g_params.animStages[SpaceStation::ANIM_DOCKING_BAY_1] = int(GetAnimValue(6) * 7.0);
		g_params.animStages[SpaceStation::ANIM_DOCKING_BAY_2] = int(GetAnimValue(7) * 7.0);
		g_params.animStages[SpaceStation::ANIM_DOCKING_BAY_3] = int(GetAnimValue(8) * 7.0);
		g_params.animStages[SpaceStation::ANIM_DOCKING_BAY_4] = int(GetAnimValue(9) * 7.0);
		g_params.animValues[SpaceStation::ANIM_DOCKING_BAY_1] = GetAnimValue(10);
		g_params.animValues[SpaceStation::ANIM_DOCKING_BAY_2] = GetAnimValue(11);
		g_params.animValues[SpaceStation::ANIM_DOCKING_BAY_3] = GetAnimValue(12);
		g_params.animValues[SpaceStation::ANIM_DOCKING_BAY_4] = GetAnimValue(13);
	}

/*
	for (int i=0; i<LMR_ARG_MAX; i++) {
		params.argDoubles[i] = GetAnimValue(i);
	}
*/

	g_params.time = gameTime;

	g_params.linthrust[0] = 2.0f * (m_linthrust[0]->GetValue() - 0.5f);
	g_params.linthrust[1] = 2.0f * (m_linthrust[1]->GetValue() - 0.5f);
	g_params.linthrust[2] = 2.0f * (m_linthrust[2]->GetValue() - 0.5f);
	g_params.angthrust[0] = 2.0f * (m_angthrust[0]->GetValue() - 0.5f);
	g_params.angthrust[1] = 2.0f * (m_angthrust[1]->GetValue() - 0.5f);
	g_params.angthrust[2] = 2.0f * (m_angthrust[2]->GetValue() - 0.5f);
}


static void render_coll_mesh(const LmrCollMesh *m)
{
	glDisable(GL_LIGHTING);
	glColor3f(1,0,1);
	glDepthRange(0.0+g_zbias,1.0);
	glBegin(GL_TRIANGLES);
	for (int i=0; i<m->ni; i+=3) {
		glVertex3fv(&m->pVertex[3*m->pIndex[i]]);
		glVertex3fv(&m->pVertex[3*m->pIndex[i+1]]);
		glVertex3fv(&m->pVertex[3*m->pIndex[i+2]]);
	}
	glEnd();
	glColor3f(1,1,1);
	glDepthRange(0,1.0f-g_zbias);
	for (int i=0; i<m->ni; i+=3) {
		glBegin(GL_LINE_LOOP);
		glVertex3fv(&m->pVertex[3*m->pIndex[i]]);
		glVertex3fv(&m->pVertex[3*m->pIndex[i+1]]);
		glVertex3fv(&m->pVertex[3*m->pIndex[i+2]]);
		glEnd();
	}
	glDepthRange(0,1);
	glEnable(GL_LIGHTING);
}

float aspectRatio = 1.0;
double camera_zoom = 1.0;
vector3f g_campos(0.0f, 0.0f, 100.0f);
matrix4x4f g_camorient;
extern int stat_rayTriIntersections;

void Viewer::MainLoop()
{
	Uint32 lastTurd = SDL_GetTicks();

	Uint32 t = SDL_GetTicks();
	int numFrames = 0, fps = 0, numTris = 0;
	Uint32 lastFpsReadout = SDL_GetTicks();
	g_campos = vector3f(0.0f, 0.0f, m_cmesh->GetBoundingRadius());
	g_camorient = matrix4x4f::Identity();
	matrix4x4f modelRot = matrix4x4f::Identity();

	printf("Geom tree build in %dms\n", SDL_GetTicks() - t);

	Render::State::SetZnearZfar(1.0f, 10000.0f);

	for (;;) {
		if (m_quit) return;
		PollEvents();
		Render::PrepareFrame();

		if (g_keyState[SDLK_LSHIFT] || g_keyState[SDLK_RSHIFT]) {
			if (g_keyState[SDLK_UP]) g_camorient = g_camorient * matrix4x4f::RotateXMatrix(g_frameTime);
			if (g_keyState[SDLK_DOWN]) g_camorient = g_camorient * matrix4x4f::RotateXMatrix(-g_frameTime);
			if (g_keyState[SDLK_LEFT]) g_camorient = g_camorient * matrix4x4f::RotateYMatrix(-g_frameTime);
			if (g_keyState[SDLK_RIGHT]) g_camorient = g_camorient * matrix4x4f::RotateYMatrix(g_frameTime);
			if (g_mouseButton[3]) {
				float rx = 0.01f*g_mouseMotion[1];
				float ry = 0.01f*g_mouseMotion[0];
				g_camorient = g_camorient * matrix4x4f::RotateXMatrix(rx);
				g_camorient = g_camorient * matrix4x4f::RotateYMatrix(ry);
				if (g_mouseButton[1]) {
					g_campos = g_campos - g_camorient * vector3f(0.0f,0.0f,1.0f) * 0.01 *
						m_model->GetDrawClipRadius();
				}
			}
		} else {
			if (g_keyState[SDLK_UP]) modelRot = modelRot * matrix4x4f::RotateXMatrix(g_frameTime);
			if (g_keyState[SDLK_DOWN]) modelRot = modelRot * matrix4x4f::RotateXMatrix(-g_frameTime);
			if (g_keyState[SDLK_LEFT]) modelRot = modelRot * matrix4x4f::RotateYMatrix(-g_frameTime);
			if (g_keyState[SDLK_RIGHT]) modelRot = modelRot * matrix4x4f::RotateYMatrix(g_frameTime);
			if (g_mouseButton[3]) {
				float rx = 0.01f*g_mouseMotion[1];
				float ry = 0.01f*g_mouseMotion[0];
				modelRot = modelRot * matrix4x4f::RotateXMatrix(rx);
				modelRot = modelRot * matrix4x4f::RotateYMatrix(ry);
			}
		}
		float rate = 1.f;
		if (g_keyState[SDLK_LSHIFT]) rate = 10.f;
		if (g_keyState[SDLK_EQUALS] || g_keyState[SDLK_KP_PLUS]) g_campos = g_campos - g_camorient * vector3f(0.0f,0.0f,1.f) * rate;
		if (g_keyState[SDLK_MINUS] || g_keyState[SDLK_KP_MINUS]) g_campos = g_campos + g_camorient * vector3f(0.0f,0.0f,1.f) * rate;
		if (g_keyState[SDLK_PAGEUP]) g_campos = g_campos - g_camorient * vector3f(0.0f,0.0f,0.5f);
		if (g_keyState[SDLK_PAGEDOWN]) g_campos = g_campos + g_camorient * vector3f(0.0f,0.0f,0.5f);

//		geom->MoveTo(modelRot, vector3d(0.0,0.0,0.0));

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		float fracH = g_height / float(g_width);
		glFrustum(-1, 1, -fracH, fracH, 1.0f, 10000.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		
		SetSbreParams();

		int beforeDrawTriStats = LmrModelGetStatsTris();
	
		if (g_renderType == 0) {
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			matrix4x4f m = g_camorient.InverseOf() * matrix4x4f::Translation(-g_campos) * modelRot.InverseOf();
			if (g_doBenchmark) {
				for (int i=0; i<1000; i++) m_model->Render(m, &g_params);
			} else {
				m_model->Render(m, &g_params);
			}
			glPopAttrib();
		} else if (g_renderType == 1) {
			glPushMatrix();
			matrix4x4f m = g_camorient.InverseOf() * matrix4x4f::Translation(-g_campos) * modelRot.InverseOf();
			glMultMatrixf(&m[0]);
			render_coll_mesh(m_cmesh);
			glPopMatrix();
		} else {
			//used to be raytrace
		}
		Render::State::UseProgram(0);
		if (m_showBoundingRadius) {
			matrix4x4f mo = g_camorient.InverseOf() * matrix4x4f::Translation(-g_campos);// * modelRot.InverseOf();
			VisualizeBoundingRadius(mo, m_model->GetDrawClipRadius());
		}
		Render::UnbindAllBuffers();

		{
			Aabb aabb = m_cmesh->GetAabb();
			m_uiManager.SetStashItem("performance.fps", stringf("FPS %0", fps));
			m_uiManager.SetStashItem("performance.triangles",
				stringf("%0 triangles, %1{f.3}m tris/sec",
				(g_renderType == 0 ?  LmrModelGetStatsTris() - beforeDrawTriStats : m_cmesh->m_numTris),
				numTris/1000000.0f));
			m_uiManager.SetStashItem("performance.dimensions",
				stringf("mesh size: %0{f.1}x%1{f.1}x%2{f.1} (radius %3{f.1})",
						aabb.max.x-aabb.min.x,
						aabb.max.y-aabb.min.y,
						aabb.max.z-aabb.min.z,
						aabb.GetBoundingRadius()));
			m_uiManager.SetStashItem("performance.radius",
				stringf("Clipping radius %0", m_model->GetDrawClipRadius()));
		}
		//testing getElement...
		std::string mname = stringf("Model: %0", m_model->GetName());
		m_ui->GetDocument()->GetElementById("modelname")->SetInnerRML(mname.c_str());
		
		Render::PostProcess();
		Gui::Draw();
		m_uiManager.Draw();
		
		glError();
		Render::SwapBuffers();
		numFrames++;
		g_frameTime = (SDL_GetTicks() - lastTurd) * 0.001f;
		lastTurd = SDL_GetTicks();

		if (SDL_GetTicks() - lastFpsReadout > 1000) {
			numTris = LmrModelGetStatsTris();
			fps = numFrames;
			numFrames = 0;
			lastFpsReadout = SDL_GetTicks();
			LmrModelClearStatsTris();
		}

		//space->Collide(onCollision);
	}
}

void Viewer::PollEvents()
{
	SDL_Event event;

	g_mouseMotion[0] = g_mouseMotion[1] = 0;
	while (SDL_PollEvent(&event)) {
		m_uiManager.HandleEvent(&event);
		Gui::HandleSDLEvent(&event);
		switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) {
                    if (m_model) {
                        PickModel();
                    } else {
                        Quit();
                    }
				}
				if (event.key.keysym.sym == SDLK_F11) SDL_WM_ToggleFullScreen(g_screen);
				if (event.key.keysym.sym == SDLK_s && (m_model)) {
					Render::ToggleShaders();
				}
				g_keyState[event.key.keysym.sym] = 1;
				break;
			case SDL_KEYUP:
				g_keyState[event.key.keysym.sym] = 0;
				break;
			case SDL_MOUSEBUTTONDOWN:
				g_mouseButton[event.button.button] = 1;
	//			Pi::onMouseButtonDown.emit(event.button.button,
	//					event.button.x, event.button.y);
				break;
			case SDL_MOUSEBUTTONUP:
				g_mouseButton[event.button.button] = 0;
	//			Pi::onMouseButtonUp.emit(event.button.button,
	//					event.button.x, event.button.y);
				break;
			case SDL_MOUSEMOTION:
				g_mouseMotion[0] += event.motion.xrel;
				g_mouseMotion[1] += event.motion.yrel;
				break;
			case SDL_QUIT:
				Quit();
				break;
		}
	}
}

void Viewer::VisualizeBoundingRadius(matrix4x4f& trans, double radius)
{
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glMultMatrixf(&trans[0]);
	glColor4f(0.5f, 0.9f, 0.9f, 1.0f);
	glColor3f(0,0,1);
	glBegin(GL_LINE_LOOP);
	for (float theta=0; theta < 2*M_PI; theta += 0.05*M_PI) {
		glVertex3f(radius*sin(theta), radius*cos(theta), 0);
	}
	glEnd();
	glPopAttrib();
	glPopMatrix();
}


int main(int argc, char **argv)
{
	if ((argc<=1) || (0==strcmp(argv[1],"--help"))) {
		printf("Usage:\nluamodelviewer <width> <height> <model name>\n");
	}
	if (argc >= 3) {
		g_width = atoi(argv[1]);
		g_height = atoi(argv[2]);
	} else {
		g_width = 800;
		g_height = 600;
	}

	const SDL_VideoInfo *info = NULL;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
		exit(-1);
	}

	info = SDL_GetVideoInfo();

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	g_zbias = 2.0/(1<<16);

	Uint32 flags = SDL_OPENGL;

	if ((g_screen = SDL_SetVideoMode(g_width, g_height, info->vfmt->BitsPerPixel, flags)) == 0) {
		// fall back on 16-bit depth buffer...
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		fprintf(stderr, "Failed to set video mode. (%s). Re-trying with 16-bit depth buffer.\n", SDL_GetError());
		if ((g_screen = SDL_SetVideoMode(g_width, g_height, info->vfmt->BitsPerPixel, flags)) == 0) {
			fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
		}
	}
	glewInit();

	glShadeModel(GL_SMOOTH);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	

	glClearColor(0,0,0,0);
	glViewport(0, 0, g_width, g_height);
	Render::Init(g_width, g_height);
	Gui::Init(g_width, g_height, g_width, g_height);
	LmrModelCompilerInit();
	LmrNotifyScreenWidth(g_width);

	ShipType::Init();

	Viewer viewer;
	if (argc >= 4) {
		try {
			LmrModel *m = LmrLookupModelByName(argv[3]);
			viewer.SetModel(m);
		} catch (LmrModelNotFoundException) {
			viewer.PickModel(argv[3], std::string("Could not find model: ") + argv[3]);
		}
	} else {
		viewer.PickModel();
	}

	viewer.MainLoop();

	Render::Uninit();
	LmrModelCompilerUninit();
	return 0;
}
