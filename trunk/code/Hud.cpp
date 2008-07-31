#include "Hud.h"
#include "d3ddefs.h"
#include "main.h"
//---------------------------------------------------------
Hud::Hud(LPDIRECT3DDEVICE9& device){
	show = true;
	font = NULL;
	tex = NULL;
	frame_counter = 0;
	last_time = 0;

	D3DXCreateTextureFromFile(device, "../art/hud1.png", &tex);
	D3DXFONT_DESC fontDesc = {16,0,600,0,false,DEFAULT_CHARSET,OUT_TT_PRECIS,
                          CLIP_DEFAULT_PRECIS,FF_MODERN,"CourierNew"};

	D3DXCreateFontIndirect(device,&fontDesc,&font);
	fontglob.left = 0;
	fontglob.right = 300;
	fontglob.top = 0;
	fontglob.bottom = 175;
	fontclan.left = 0;
	fontclan.right = 300;
	fontclan.top = 175;
	fontclan.bottom = 350;
	fontgroup.left = 0;
	fontgroup.right = 300;
	fontgroup.top = 470;
	fontgroup.bottom = (LONG)(g_winy*0.99f);
	fontsp.left = 300;
	fontsp.right = 495;
	fontsp.top = 590;
	fontsp.bottom = 620;

	subpopup = false;

	sp_vb = NULL;
	sp = new s_vert2dc[9];
	sp[0].set(500, 590, 0xff000000);
	sp[1].set(500, 615, 0xff000000);
	sp[2].set(300, 590, 0xff000000);
	sp[3].set(300, 615, 0xff000000);
	sp[4].set(500, 590, 0xffffffff);
	sp[5].set(500, 615, 0xffffffff);
	sp[6].set(300, 615, 0xffffffff);
	sp[7].set(300, 590, 0xffffffff);
	sp[8].set(500, 590, 0xffffffff);
	g_device->CreateVertexBuffer(sizeof(s_vert2dc)*9,D3DUSAGE_WRITEONLY,VERT2DC,D3DPOOL_DEFAULT,&sp_vb,NULL);
	sp_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,sp,sizeof(s_vert2dc)*9);
	sp_vb->Unlock();


	back_vb = NULL;
	back = new s_vert2dc[4];
	back[0].set(300,0,0xFF000000);
	back[1].set(300,(float)g_winy,0xFF000000);
	back[2].set(0,0,0xFF000000);
	back[3].set(0,(float)g_winy,0xFF000000);
	VOID* pData;
	g_device->CreateVertexBuffer(sizeof(s_vert2dc)*4,D3DUSAGE_WRITEONLY,VERT2DC,D3DPOOL_DEFAULT,&back_vb,NULL);
	back_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,back,sizeof(s_vert2dc)*4);
	back_vb->Unlock();

	lines_vb = NULL;
	lnb = 20;
	lines = new s_vert2dc[300]; /*----!----*/
	// Panel frame
	lines[0].set(300,0,0xFFFFFFFF);
	lines[1].set(300,(float)g_winy,0xFFFFFFFF);

	// Global panel
	lines[2].set(5,40,0xFFFFFFFF);
	lines[3].set(40,40,0xFFFFFFFF);

	lines[4].set(40,30,0xFFFFFFFF);
	lines[5].set(40,50,0xFFFFFFFF);
	lines[6].set(120,40,0xFFFFFFFF);
	lines[7].set(295,40,0xFFFFFFFF);
	lines[8].set(120,30,0xFFFFFFFF);
	lines[9].set(120,50,0xFFFFFFFF);

	lines[16].set(40,30,0xFF00FF00);
	lines[17].set(120,30,0xFFFFFF00);
	lines[18].set(40,50,0xFF00FF00);
	lines[19].set(120,50,0xFFFFFF00);

	lines[10].set(295,40,0xFFFFFFFF);
	lines[11].set(295,140,0xFFFFFFFF);
	lines[12].set(5,40,0xFFFFFFFF);
	lines[13].set(5,140,0xFFFFFFFF);
	lines[14].set(5,140,0xFFFFFFFF);
	lines[15].set(295,140,0xFFFFFFFF);


	// Clan panel
	lines[lnb++].set(5,200,0xFFFFFFFF);
	lines[lnb++].set(40,200,0xFFFFFFFF);
	lines[lnb++].set(100,200,0xFFFFFFFF);
	lines[lnb++].set(295,200,0xFFFFFFFF);
	
	lines[lnb++].set(40,190,0xFF00FF00);
	lines[lnb++].set(100,190,0xFFFFFF00);
	lines[lnb++].set(40,210,0xFF00FF00);
	lines[lnb++].set(100,210,0xFFFFFF00);
	
	lines[lnb++].set(40,190,0xFFFFFFFF);
	lines[lnb++].set(40,210,0xFFFFFFFF);
	lines[lnb++].set(100,190,0xFFFFFFFF);
	lines[lnb++].set(100,210,0xFFFFFFFF);

	lines[lnb++].set(5,200,0xFFFFFFFF);
	lines[lnb++].set(5,440,0xFFFFFFFF);
	lines[lnb++].set(295,200,0xFFFFFFFF);
	lines[lnb++].set(295,440,0xFFFFFFFF);

	lines[lnb++].set(5,440,0xFFFFFFFF);
	lines[lnb++].set(295,440,0xFFFFFFFF);

	// Group panel
	lines[lnb++].set(5,495,0xFFFFFFFF);
	lines[lnb++].set(40,495,0xFFFFFFFF);
	lines[lnb++].set(295,495,0xFFFFFFFF);
	lines[lnb++].set(120,495,0xFFFFFFFF);

	lines[lnb++].set(40,505,0xFF00FF00);
	lines[lnb++].set(120,505,0xFFFFFF00);
	lines[lnb++].set(40,485,0xFF00FF00);
	lines[lnb++].set(120,485,0xFFFFFF00);

	lines[lnb++].set(40,485,0xFFFFFFFF);
	lines[lnb++].set(40,505,0xFFFFFFFF);
	lines[lnb++].set(120,505,0xFFFFFFFF);
	lines[lnb++].set(120,485,0xFFFFFFFF);

	lines[lnb++].set(5,495,0xFFFFFFFF);
	lines[lnb++].set(5,(float)g_winy-50,0xFFFFFFFF);
	lines[lnb++].set(295,495,0xFFFFFFFF);
	lines[lnb++].set(295,(float)g_winy-50,0xFFFFFFFF);

	// subgroup
	lines[lnb++].set(60,560,0xFF5555FF);
	lines[lnb++].set(60,590,0xFF5555FF);
	lines[lnb++].set(240,560,0xFF5555FF);
	lines[lnb++].set(240,590,0xFF5555FF);
	lines[lnb++].set(60,560,0xFF5555FF);
	lines[lnb++].set(240,560,0xFF5555FF);
	lines[lnb++].set(60,590,0xFF5555FF);
	lines[lnb++].set(240,590,0xFF5555FF);

	
	// Uparrow
	lines[lnb++].set(130,645,0xFF5555FF);
	lines[lnb++].set(150,635,0xFF5555FF);
	lines[lnb++].set(150,635,0xFF5555FF);
	lines[lnb++].set(170,645,0xFF5555FF);

	// Downarrow
	lines[lnb++].set(130,(float)g_winy-65,0xFF5555FF);
	lines[lnb++].set(150,(float)g_winy-55,0xFF5555FF);
	lines[lnb++].set(150,(float)g_winy-55,0xFF5555FF);
	lines[lnb++].set(170,(float)g_winy-65,0xFF5555FF);

	// List up
	lines[lnb++].set(5,630,0xFFFFFFFF);
	lines[lnb++].set(295,630,0xFFFFFFFF);
	lines[lnb++].set(5,650,0xFF555555);
	lines[lnb++].set(295,650,0xFF555555);

	// List Down
	lines[lnb++].set(5,(float)g_winy-70,0xFF555555);
	lines[lnb++].set(295,(float)g_winy-70,0xFF555555);
	lines[lnb++].set(5,(float)g_winy-50,0xFFFFFFFF);
	lines[lnb++].set(295,(float)g_winy-50,0xFFFFFFFF);

	// List plus & minus
	lines[lnb++].set(259,640,0xFF5555FF);
	lines[lnb++].set(271,640,0xFF5555FF);
	lines[lnb++].set(279,640,0xFF5555FF);
	lines[lnb++].set(291,640,0xFF5555FF);
	lines[lnb++].set(285,634,0xFF5555FF);
	lines[lnb++].set(285,646,0xFF5555FF);

	lines[lnb++].set(275,630,0xFFAAAAAA);
	lines[lnb++].set(275,650,0xFFAAAAAA);
	lines[lnb++].set(255,630,0xFFAAAAAA);
	lines[lnb++].set(255,650,0xFFAAAAAA);

	// End Turn
	lines[lnb++].set(170,(float)g_winy-8,0xFF5555FF);
	lines[lnb++].set(275,(float)g_winy-8,0xFF5555FF);
	lines[lnb++].set(170,(float)g_winy-30,0xFF5555FF);
	lines[lnb++].set(275,(float)g_winy-30,0xFF5555FF);
	lines[lnb++].set(170,(float)g_winy-8,0xFF5555FF);
	lines[lnb++].set(170,(float)g_winy-30,0xFF5555FF);
	lines[lnb++].set(275,(float)g_winy-8,0xFF5555FF);
	lines[lnb++].set(275,(float)g_winy-30,0xFF5555FF);


	g_device->CreateVertexBuffer(sizeof(s_vert2dc)*lnb,D3DUSAGE_WRITEONLY,VERT2DC,D3DPOOL_DEFAULT,&lines_vb,NULL);
	lines_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,lines,sizeof(s_vert2dc)*lnb);
	lines_vb->Unlock();

	listspan = g_winy-720;
	listscale = 5;
	listgap = (float)(listspan/listscale);
	listoffset = 0;
	stam = NULL;
	qstam = NULL;
	snb = 0;
	stam_vb = NULL;
	qsnb = 0;
	qstam_vb = NULL;

	UpdateSelect();
}
//---------------------------------------------------------
Hud::~Hud(){
	if (back_vb) back_vb->Release();
	if (lines_vb) lines_vb->Release();
	if (stam_vb) stam_vb->Release();
	if (qstam_vb) qstam_vb->Release();
	if (tex) tex->Release();
	if (font) font->Release();
	delete back;
	delete lines;
	if (stam) delete stam;
	if (qstam) delete qstam;
}
//---------------------------------------------------------
void Hud::Click(int x, int y){
	if (y >= 560){
		if (y<=590 && x>=60 && x<=240 && g_nbSelected==1){
			subpopup = !subpopup;
			spstr[0] = '|';
			spstr[1] = 0;
			spnum = 0;
		} else if (y>=g_winy-30 && y<=g_winy-8 && x>=170 && x<=275){
			g_game.Turn();
		} else if (y>=630 && y<=650){
			if (x>255){
				if (x < 275){
					listscale -= listscale>1 ? 1 : 0;
					listgap = (float)(listspan/listscale);
				} else {
					listscale += listscale<20 ? 1 : 0;
					listgap = (float)(listspan/listscale);
				}
			} else {
				listoffset -= listoffset>0 ? 1 : 0;
			}
			UpdateSelect();
		} else if (y>=g_winy-70 && y<=g_winy-50){
			listoffset += listoffset==g_nbSelected ? 0 : 1;
			UpdateSelect();
		}
	}
}
//---------------------------------------------------------
void Hud::Update(float time, float dt){
	if (time - last_time > 1.0f) {
		sprintf(fps,FPSFROMAT,frame_counter);
		frame_counter = 0;
		last_time = time;
	}
}
//---------------------------------------------------------
void Hud::UpdateSelect(){
	sprintf(group,GROUPFROMAT,g_nbSelected, g_selectedpop);

	if (stam) delete stam;
	if (qstam) delete qstam;
	snb = 0;
	qsnb = 0;
	stam = new s_vert2dc[listscale*8];
	qstam = new s_vert2dc[listscale*12];
	if (listoffset+listscale > g_nbSelected) listoffset = g_nbSelected-listscale;
	if (g_nbSelected <= listscale) listoffset = 0;
	float ilg, rr, rl;
	for (double i=listoffset; i<listscale+listoffset; ++i){
		if (i+listoffset>=g_nbSelected) break;
		ilg = (float)((i-listoffset)*listgap);
		stam[snb++].set(10,655+ilg,0xFFAAAAAA);
		stam[snb++].set(290,655+ilg,0xFFAAAAAA);
		stam[snb++].set(10,645+ilg+listgap,0xFFAAAAAA);
		stam[snb++].set(290,645+ilg+listgap,0xFFAAAAAA);
		stam[snb++].set(10,655+ilg,0xFFAAAAAA);
		stam[snb++].set(10,645+ilg+listgap,0xFFAAAAAA);
		stam[snb++].set(290,655+ilg,0xFFAAAAAA);
		stam[snb++].set(290,645+ilg+listgap,0xFFAAAAAA);


		Group* g = g_selected[(int)(i)];
		if (g->stamina > 0){
			rr = 280.0f*(float)((g->clan->stamina-g->stamina)/g->clan->stamina);
			qstam[qsnb++].set(290-rr,655+ilg,0xFF0080FF);
			qstam[qsnb++].set(290-rr,645+ilg+listgap,0xFF0080FF);
			qstam[qsnb++].set(10,655+ilg,0xFF0080FF);
			qstam[qsnb++].set(10,655+ilg,0xFF0080FF);
			qstam[qsnb++].set(290-rr,645+ilg+listgap,0xFF0080FF);
			qstam[qsnb++].set(10,645+ilg+listgap,0xFF0080FF);
			if (g->planed+g_game.nextcost[(int)(i)]){
				rl = (280.0f-rr)*(float)((g->stamina-(g->planed+g_game.nextcost[(int)(i)]))/g->stamina);
				qstam[qsnb++].set(290-rr,655+ilg,0xFFFF2000);
				qstam[qsnb++].set(290-rr,645+ilg+listgap,0xFFFF2000);
				qstam[qsnb++].set(10+rl,655+ilg,0xFFFF2000);
				qstam[qsnb++].set(10+rl,655+ilg,0xFFFF2000);
				qstam[qsnb++].set(290-rr,645+ilg+listgap,0xFFFF2000);
				qstam[qsnb++].set(10+rl,645+ilg+listgap,0xFFFF2000);
			}
		}
	}

	if (stam_vb) stam_vb->Release();
	g_device->CreateVertexBuffer(sizeof(s_vert2dc)*(snb+2),D3DUSAGE_WRITEONLY,VERT2DC,D3DPOOL_DEFAULT,&stam_vb,NULL);
	stam_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,stam,sizeof(s_vert2dc)*snb);
	stam_vb->Unlock();	
	if (qstam_vb) qstam_vb->Release();
	g_device->CreateVertexBuffer(sizeof(s_vert2dc)*(qsnb+2),D3DUSAGE_WRITEONLY,VERT2DC,D3DPOOL_DEFAULT,&qstam_vb,NULL);
	qstam_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,qstam,sizeof(s_vert2dc)*qsnb);
	qstam_vb->Unlock();	
}
//---------------------------------------------------------
void Hud::UpdateText(){
	sprintf(glob,GLOBFROMAT,g_nbClans, g_pop);
	Clan& c = *g_clan;
	sprintf(clan,CLANFROMAT,c.size, c.nbGroups, c.gold, c.stamina, c.attack, c.defense, c.culture);
	sprintf(turn,TURNFROMAT,g_turn);
}
//---------------------------------------------------------
void Hud::PopupEnter(){
	if (spnum){
		g_selected[0]->Subgroup(spnum);
		g_selectedpop = spnum;
		UpdateText();
		UpdateSelect();
	}
	subpopup = false;
}
//---------------------------------------------------------
void Hud::PopupDelete(){
	int i;
	for (i=0; spstr[i]; ++i);
	if (i>1){
		spstr[i-2] = '|';
		spstr[i-1] = 0;
		spnum /= 10;
	}
}
//---------------------------------------------------------
void Hud::PopupText(int key){
	double nextnum = spnum*10 + key;
	if (nextnum < g_selectedpop){
		int i;
		for (i=0; spstr[i]; ++i);
		spstr[i-1] = '0'+key;
		spstr[i] = '|';
		spstr[i+1] = 0;
		spnum = nextnum;
	}
}
//---------------------------------------------------------
void Hud::RenderTextures(){
}
//---------------------------------------------------------
void Hud::RenderText(){
	font->DrawText(NULL,fps,-1,&fontglob, DT_TOP | DT_LEFT | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,"\n\n     Global",-1,&fontglob, DT_LEFT | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,glob,-1,&fontglob, DT_CENTER | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,"\n     Clan",-1,&fontclan, DT_LEFT | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,clan,-1,&fontclan, DT_CENTER | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,"\n     Groups",-1,&fontgroup, DT_LEFT | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,group,-1,&fontgroup, DT_CENTER | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,turn,-1,&fontgroup, DT_BOTTOM | DT_CENTER | DT_NOCLIP,0xffffffff);
}
//---------------------------------------------------------
void Hud::Render(){
	if (show) {
		g_device->SetFVF(VERT2DC);

		g_device->SetStreamSource(0,back_vb,0,sizeof(s_vert2dc));
		g_device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);

		RenderTextures();
	
		g_device->SetStreamSource(0,qstam_vb,0,sizeof(s_vert2dc));
		g_device->DrawPrimitive(D3DPT_TRIANGLELIST,0,qsnb/3);

		g_device->SetStreamSource(0,stam_vb,0,sizeof(s_vert2dc));
		g_device->DrawPrimitive(D3DPT_LINELIST,0,snb/2);

		g_device->SetStreamSource(0,lines_vb,0,sizeof(s_vert2dc));
		g_device->DrawPrimitive(D3DPT_LINELIST,0,lnb/2);
		
		if (subpopup){
			g_device->SetStreamSource(0,sp_vb,0,sizeof(s_vert2dc));
			g_device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
			g_device->SetStreamSource(0,sp_vb,sizeof(s_vert2dc)*4,sizeof(s_vert2dc));
			g_device->DrawPrimitive(D3DPT_LINESTRIP,0,4);
			font->DrawText(NULL,spstr,-1,&fontsp, DT_TOP | DT_RIGHT | DT_NOCLIP,0xffffffff);
		}

		RenderText();
	}




	frame_counter++;
}
//---------------------------------------------------------
void Hud::Toggle(){
	show = !show;
}
