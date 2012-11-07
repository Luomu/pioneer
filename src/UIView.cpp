// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "UIView.h"
#include "Pi.h"
#include "ui/Context.h"
#include "gameui/Panel.h"
#include "graphics/RenderTarget.h"
#include "WorldView.h"

void UIView::Update()
{
	Pi::ui->Update();
}

void UIView::Draw3D()
{
	Graphics::RenderTarget* rt = 0;
	if (Pi::renderer->GetRenderTarget()) {
		rt = static_cast<Graphics::RenderTarget*>(Pi::renderer->GetRenderTarget());
		rt->BeginRTT();
		Pi::worldView->Update();
		Pi::worldView->Draw3D();
		rt->EndRTT();
		//downsample+blur to a temporary texture
		//downsample+blur to a temporary texture
		//set as ui background texture
	}
	Pi::ui->Draw();
}

void UIView::OnSwitchTo()
{
	Pi::ui->SetInnerWidget(
		Pi::ui->VBox()
			->PackEnd(Pi::ui->CallTemplate(m_templateName))
			->PackEnd(new GameUI::Panel(Pi::ui.Get()))
	);
}

void UIView::OnSwitchFrom()
{
	Pi::ui->RemoveInnerWidget();
}
