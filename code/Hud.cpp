#include "Hud.h"
#include "d3ddefs.h"
#include "main.h"
//---------------------------------------------------------
static char*apstrs[4] = {
	"\n\n\n\n\n",
	"\n\n\n\n\n\n\n\n\n",
	"\n\n\n\n\n\n\n\n\n\n\n\n\n",
	"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
};
static double attackbonus[] = {
	0.0, 1.0, 1.2, 1.5, 2.0, 2.6, 3.3, 4.0, 5.0
};
//---------------------------------------------------------
Hud::Hud(LPDIRECT3DDEVICE9& device){
	show = true;
	caption = false;
	font = NULL;
	tex = NULL;
	frame_counter = 0;
	last_time = 0;

	D3DXCreateTextureFromFile(device, "../art/hud.png", &tex);
	texpts = new s_vert2dt[12];
	texpts[0].set(294,655,0.56f,0);
	texpts[1].set(294,970,0.56f,0.6f);
	texpts[2].set(6,655,0,0);
	texpts[3].set(6,655,0,0);
	texpts[4].set(294,970,0.56f,0.6f);
	texpts[5].set(6,970,0,0.6f);

	texpts[6].set(293,397,1,0.8125f);
	texpts[7].set(293,493,1,0.999f);
	texpts[8].set(7,397,0.4425f,0.8125f);
	texpts[9].set(7,397,0.4425f,0.8125f);
	texpts[10].set(293,493,1,0.999f);
	texpts[11].set(7,493,0.4425f,0.999f);

	g_device->CreateVertexBuffer(sizeof(s_vert2dt)*12,D3DUSAGE_WRITEONLY,VERT2DT,D3DPOOL_DEFAULT,&tex_vb,NULL);
	tex_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,texpts,sizeof(s_vert2dt)*12);
	tex_vb->Unlock();

	D3DXFONT_DESC fontDesc = {16,0,600,0,false,DEFAULT_CHARSET,OUT_TT_PRECIS,
                          CLIP_DEFAULT_PRECIS,FF_MODERN,"CourierNew"};

	D3DXCreateFontIndirect(device,&fontDesc,&font);
	fontglob.left = 0;
	fontglob.right = 300;
	fontglob.top = 7;
	fontglob.bottom = 175;
	fontfed.left = 0;
	fontfed.right = 300;
	fontfed.top = 101;
	fontfed.bottom = 400;
	fontclan.left = 0;
	fontclan.right = 300;
	fontclan.top = 220;
	fontclan.bottom = 500;
	fontgroup.left = 0;
	fontgroup.right = 300;
	fontgroup.top = 485;
	fontgroup.bottom = (LONG)(g_winy*0.99f);
	fontsp.left = 300;
	fontsp.right = 495;
	fontsp.top = 590;
	fontsp.bottom = 620;
	fontcaption.left = 310;
	fontcaption.right = (LONG)(g_winx);
	fontcaption.top = 5;
	fontcaption.bottom = (LONG)(g_winy*0.99f);

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
	back[1].set(300,g_winy,0xFF000000);
	back[2].set(0,0,0xFF000000);
	back[3].set(0,g_winy,0xFF000000);
	VOID* pData;
	g_device->CreateVertexBuffer(sizeof(s_vert2dc)*4,D3DUSAGE_WRITEONLY,VERT2DC,D3DPOOL_DEFAULT,&back_vb,NULL);
	back_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,back,sizeof(s_vert2dc)*4);
	back_vb->Unlock();

	lines_vb = NULL;
	lnb = 0;
	lines = new s_vert2dc[300]; /*----!----*/
	// Panel frame
	lines[lnb++].set(300,0,0xFFFFFFFF);
	lines[lnb++].set(300,g_winy,0xFFFFFFFF);

	// Global panel
	lines[lnb++].set(5,15,0xFFFFFFFF);
	lines[lnb++].set(40,15,0xFFFFFFFF);

	lines[lnb++].set(40,5,0xFFFFFFFF);
	lines[lnb++].set(40,25,0xFFFFFFFF);
	lines[lnb++].set(120,15,0xFFFFFFFF);
	lines[lnb++].set(295,15,0xFFFFFFFF);
	lines[lnb++].set(120,5,0xFFFFFFFF);
	lines[lnb++].set(120,25,0xFFFFFFFF);

	lines[lnb++].set(40,5,0xFF00FF00);
	lines[lnb++].set(120,5,0xFFFFBB00);
	lines[lnb++].set(40,25,0xFF00FF00);
	lines[lnb++].set(120,25,0xFFFFBB00);

	lines[lnb++].set(295,15,0xFFFFFFFF);
	lines[lnb++].set(295,100,0xFFFFFFFF);
	lines[lnb++].set(5,15,0xFFFFFFFF);
	lines[lnb++].set(5,100,0xFFFFFFFF);
	lines[lnb++].set(5,100,0xFFFFFFFF);
	lines[lnb++].set(295,100,0xFFFFFFFF);

	// Federation panel
	lines[lnb++].set(5,125,0xFFFFFFFF);
	lines[lnb++].set(40,125,0xFFFFFFFF);

	lines[lnb++].set(40,115,0xFFFFFFFF);
	lines[lnb++].set(40,135,0xFFFFFFFF);
	lines[lnb++].set(160,125,0xFFFFFFFF);
	lines[lnb++].set(295,125,0xFFFFFFFF);
	lines[lnb++].set(160,115,0xFFFFFFFF);
	lines[lnb++].set(160,135,0xFFFFFFFF);

	lines[lnb++].set(40,115,0xFF00FF00);
	lines[lnb++].set(160,115,0xFFFFBB00);
	lines[lnb++].set(40,135,0xFF00FF00);
	lines[lnb++].set(160,135,0xFFFFBB00);

	lines[lnb++].set(295,125,0xFFFFFFFF);
	lines[lnb++].set(295,220,0xFFFFFFFF);
	lines[lnb++].set(5,125,0xFFFFFFFF);
	lines[lnb++].set(5,220,0xFFFFFFFF);
	lines[lnb++].set(5,220,0xFFFFFFFF);
	lines[lnb++].set(295,220,0xFFFFFFFF);

	// Clan panel
	lines[lnb++].set(5,245,0xFFFFFFFF);
	lines[lnb++].set(40,245,0xFFFFFFFF);
	lines[lnb++].set(100,245,0xFFFFFFFF);
	lines[lnb++].set(295,245,0xFFFFFFFF);
	
	lines[lnb++].set(40,235,0xFF00FF00);
	lines[lnb++].set(100,235,0xFFFFBB00);
	lines[lnb++].set(40,255,0xFF00FF00);
	lines[lnb++].set(100,255,0xFFFFBB00);
	
	lines[lnb++].set(40,235,0xFFFFFFFF);
	lines[lnb++].set(40,255,0xFFFFFFFF);
	lines[lnb++].set(100,235,0xFFFFFFFF);
	lines[lnb++].set(100,255,0xFFFFFFFF);

	lines[lnb++].set(5,245,0xFFFFFFFF);
	lines[lnb++].set(5,390,0xFFFFFFFF);
	lines[lnb++].set(295,245,0xFFFFFFFF);
	lines[lnb++].set(295,390,0xFFFFFFFF);

	lines[lnb++].set(5,390,0xFFFFFFFF);
	lines[lnb++].set(295,390,0xFFFFFFFF);

	// Group panel
	lines[lnb++].set(5,510,0xFFFFFFFF);
	lines[lnb++].set(40,510,0xFFFFFFFF);
	lines[lnb++].set(295,510,0xFFFFFFFF);
	lines[lnb++].set(120,510,0xFFFFFFFF);

	lines[lnb++].set(40,520,0xFF00FF00);
	lines[lnb++].set(120,520,0xFFFFBB00);
	lines[lnb++].set(40,500,0xFF00FF00);
	lines[lnb++].set(120,500,0xFFFFBB00);

	lines[lnb++].set(40,500,0xFFFFFFFF);
	lines[lnb++].set(40,520,0xFFFFFFFF);
	lines[lnb++].set(120,520,0xFFFFFFFF);
	lines[lnb++].set(120,500,0xFFFFFFFF);

	lines[lnb++].set(5,510,0xFFFFFFFF);
	lines[lnb++].set(5,g_winy-50,0xFFFFFFFF);
	lines[lnb++].set(295,510,0xFFFFFFFF);
	lines[lnb++].set(295,g_winy-50,0xFFFFFFFF);

	// subgroup
	lines[lnb++].set(60,575,0xFF5555FF);
	lines[lnb++].set(60,605,0xFF5555FF);
	lines[lnb++].set(240,575,0xFF5555FF);
	lines[lnb++].set(240,605,0xFF5555FF);
	lines[lnb++].set(60,575,0xFF5555FF);
	lines[lnb++].set(240,575,0xFF5555FF);
	lines[lnb++].set(60,605,0xFF5555FF);
	lines[lnb++].set(240,605,0xFF5555FF);

	
	// Uparrow
	lines[lnb++].set(130,645,0xFF5555FF);
	lines[lnb++].set(150,635,0xFF5555FF);
	lines[lnb++].set(150,635,0xFF5555FF);
	lines[lnb++].set(170,645,0xFF5555FF);

	// Downarrow
	lines[lnb++].set(130,g_winy-65,0xFF5555FF);
	lines[lnb++].set(150,g_winy-55,0xFF5555FF);
	lines[lnb++].set(150,g_winy-55,0xFF5555FF);
	lines[lnb++].set(170,g_winy-65,0xFF5555FF);

	// List up
	lines[lnb++].set(5,630,0xFFFFFFFF);
	lines[lnb++].set(295,630,0xFFFFFFFF);
	lines[lnb++].set(5,650,0xFF555555);
	lines[lnb++].set(295,650,0xFF555555);

	// List Down
	lines[lnb++].set(5,g_winy-70,0xFF555555);
	lines[lnb++].set(295,g_winy-70,0xFF555555);
	lines[lnb++].set(5,g_winy-50,0xFFFFFFFF);
	lines[lnb++].set(295,g_winy-50,0xFFFFFFFF);

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
	lines[lnb++].set(170,g_winy-8,0xFF5555FF);
	lines[lnb++].set(275,g_winy-8,0xFF5555FF);
	lines[lnb++].set(170,g_winy-30,0xFF5555FF);
	lines[lnb++].set(275,g_winy-30,0xFF5555FF);
	lines[lnb++].set(170,g_winy-8,0xFF5555FF);
	lines[lnb++].set(170,g_winy-30,0xFF5555FF);
	lines[lnb++].set(275,g_winy-8,0xFF5555FF);
	lines[lnb++].set(275,g_winy-30,0xFF5555FF);


	g_device->CreateVertexBuffer(sizeof(s_vert2dc)*lnb,D3DUSAGE_WRITEONLY,VERT2DC,D3DPOOL_DEFAULT,&lines_vb,NULL);
	lines_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,lines,sizeof(s_vert2dc)*lnb);
	lines_vb->Unlock();

	listspan = (int)g_winy-720;
	listscale = 5;
	listgap = (float)(listspan/listscale);
	listoffset = 0;
	stam = NULL;
	qstam = NULL;
	snb = 0;
	stam_vb = NULL;
	qsnb = 0;
	qstam_vb = NULL;

	attackpanel = false;
	resultpanel = false;
	fontap.left = (LONG)(g_winx/2-400+150);
	fontap.right = (LONG)(g_winx/2+400+150);
	fontap.top = (LONG)(g_winy/2-250);
	fontap.bottom = (LONG)(g_winy/2+170);
	ap_vb = NULL;
	ap = new s_vert2dc[24];
	ap[0].set(g_winx/2+500+150, g_winy/2-300, 0xFF000000);
	ap[1].set(g_winx/2+500+150, g_winy/2+200, 0xFF000000);
	ap[2].set(g_winx/2-500+150, g_winy/2-300, 0xFF000000);
	ap[3].set(g_winx/2-500+150, g_winy/2+200, 0xFF000000);

	ap[4].set(g_winx/2+500+150, g_winy/2-300, 0xFFFFFFFF);
	ap[5].set(g_winx/2+500+150, g_winy/2+200, 0xFFFFFFFF);
	ap[6].set(g_winx/2-500+150, g_winy/2+200, 0xFFFFFFFF);
	ap[7].set(g_winx/2-500+150, g_winy/2-300, 0xFFFFFFFF);
	ap[8].set(g_winx/2+500+150, g_winy/2-300, 0xFFFFFFFF);

	ap[9].set(g_winx/2+20, g_winy/2+180, 0xFFFFFFFF);
	ap[10].set(g_winx/2+105, g_winy/2+180, 0xFFFFFFFF);
	ap[11].set(g_winx/2+105, g_winy/2+145, 0xFFFFFFFF);
	ap[12].set(g_winx/2+20, g_winy/2+145, 0xFFFFFFFF);
	ap[13].set(g_winx/2+20, g_winy/2+180, 0xFFFFFFFF);

	ap[14].set(g_winx/2+20+173, g_winy/2+180, 0xFFFFFFFF);
	ap[15].set(g_winx/2+105+173, g_winy/2+180, 0xFFFFFFFF);
	ap[16].set(g_winx/2+105+173, g_winy/2+145, 0xFFFFFFFF);
	ap[17].set(g_winx/2+20+173, g_winy/2+145, 0xFFFFFFFF);
	ap[18].set(g_winx/2+20+173, g_winy/2+180, 0xFFFFFFFF);

	ap[19].set(g_winx/2+120, g_winy/2+180, 0xFFFFFFFF);
	ap[20].set(g_winx/2+180, g_winy/2+180, 0xFFFFFFFF);
	ap[21].set(g_winx/2+180, g_winy/2+145, 0xFFFFFFFF);
	ap[22].set(g_winx/2+120, g_winy/2+145, 0xFFFFFFFF);
	ap[23].set(g_winx/2+120, g_winy/2+180, 0xFFFFFFFF);

	g_device->CreateVertexBuffer(sizeof(s_vert2dc)*24,D3DUSAGE_WRITEONLY,VERT2DC,D3DPOOL_DEFAULT,&ap_vb,NULL);
	ap_vb->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,ap,sizeof(s_vert2dc)*24);
	ap_vb->Unlock();

	UpdateSelect();
}
//---------------------------------------------------------
Hud::~Hud(){
	if (back_vb) back_vb->Release();
	if (lines_vb) lines_vb->Release();
	if (stam_vb) stam_vb->Release();
	if (qstam_vb) qstam_vb->Release();
	if (tex_vb) tex_vb->Release();
	if (tex) tex->Release();
	if (font) font->Release();
	delete texpts;
	delete back;
	delete lines;
	if (stam) delete stam;
	if (qstam) delete qstam;
}
//---------------------------------------------------------
void Hud::Click(int x, int y){
	Group* g;
	if (attackpanel && x>300){
		if (y>=g_winy/2+145 && y<=g_winy/2+180){
			// Inside Cancel/Attack buttons area
			if (x>=g_winx/2+20 && x<=g_winx/2+105){
				// Cancel
				attackpanel = false;
				SetCaption("Attack cancelled.");
			} else if (x>=g_winx/2+193 && x<=g_winx/2+278) {
				// Attack
				resultpanel = true;
				; /// !!! ///
			} else if (resultpanel && x>=g_winx/2+120 && x<=g_winx/2+180) {
				// Close button
				resultpanel = false;
				attackpanel = false;
			}
		} else if (x>=1360 && x<=88){
			// Stamina buttons
			; /// !!! ///
		}
	}
	if (y >= 397 && y <= 493){
		if (x<104){
			// Dig
			if (g_nbSelected==1){
				g = g_selected[0];
				if (g_map->gold[g->x*g_side+g->z]){
					g->mining = !g->mining;
					SetCaption(g->mining ? "Now mining." : "Stopped mining.");
				} else {
					SetCaption("No gold here, can't mine.");
				}
			} else {
				SetCaption(g_nbSelected ? "Too many groups selected." : "No group selected.");
			}
		} else if (x>196) {
			// Attack
			if (g_nbSelected){
				g_game.attacking = !g_game.attacking;
				if (attackpanel){
					attackpanel = false;
					SetCaption("Attack cancelled.");
				}
				SetCaption(g_game.attacking ? "Select group to attack." : "Attack cancelled.");
			} else {
				if (g_game.attacking){
					g_game.attacking = false;
					SetCaption("Attack cancelled.");
				} else {
					SetCaption("No group selected.");
				}
			}
		} else {
			// Gold
			;
		}
	} else if (y >= 560){
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
void Hud::AttackPanel(int x, int z){
	attackpanel = true;
	Group* g = NULL;
	Group* ag = g_board[x*g_side+z];
	sprintf(attacktitle, "\nAttacking %s group of %.0f units", ag->clan->name, ag->size);
	apow = 0;
	for (int i=g_nbSelected-1; i>=0; --i){
		g = g_selected[i];
		attackstam[i] = g->stamina-g->planed;
		apow += attackstam[i]*g->size;
		sprintf(attackstr[i][0], "%sGroup %d: %.0f units with %.2f stamina available.", apstrs[i] , i+1, g->size, g->stamina-g->planed);
		sprintf(attackstr[i][1], "%s+++ \nStamina to use: %3.0f%%\n--- ", apstrs[i]+1, (attackstam[i]*100)/g->stamina-g->planed);
	}
	sprintf(attackrez, "%s%s       Total attack power: %.2f x %.0f x %.1f = %.2f", apstrs[3], apstrs[0], apow, g->clan->attack, attackbonus[g_nbSelected], apow * attackbonus[g_nbSelected] * g->clan->attack);
	apow *= attackbonus[g_nbSelected] * g->clan->attack;
}
//---------------------------------------------------------
void Hud::SetCaption(char* str){
	captionstr = str;
	caption = true;
}
//---------------------------------------------------------
void Hud::CaptionOff(){
	caption = false;
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
	Clan& c = *g_clan;
	sprintf(glob,GLOBFROMAT,g_nbClans, g_pop, g_nbFeds);
	sprintf(fed,FEDFROMAT, 1, c.size, (c.size*100)/g_pop);
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
	g_device->SetTexture(0,tex);
	g_device->SetStreamSource(0,tex_vb,0,sizeof(s_vert2dt));
	g_device->DrawPrimitive(D3DPT_TRIANGLELIST,0,4);
	g_device->SetTexture(0,NULL);
}
//---------------------------------------------------------
void Hud::RenderText(){
	font->DrawText(NULL,fps,-1,&fontcaption, DT_TOP | DT_LEFT | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,"     Global",-1,&fontglob, DT_LEFT | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,glob,-1,&fontglob, DT_CENTER | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,"\n     Federation",-1,&fontfed, DT_LEFT | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,fed,-1,&fontfed, DT_CENTER | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,"\n     Clan",-1,&fontclan, DT_LEFT | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,clan,-1,&fontclan, DT_CENTER | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,"\n     Groups",-1,&fontgroup, DT_LEFT | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,group,-1,&fontgroup, DT_CENTER | DT_NOCLIP,0xffffffff);
	font->DrawText(NULL,turn,-1,&fontgroup, DT_BOTTOM | DT_CENTER | DT_NOCLIP,0xffffffff);
	if (caption) font->DrawText(NULL,captionstr,-1,&fontcaption, DT_BOTTOM | DT_LEFT | DT_NOCLIP,0xffffffff);
}
//---------------------------------------------------------
void Hud::Render(){
	if (show) {
	
		g_device->SetFVF(VERT2DC);
		g_device->SetStreamSource(0,back_vb,0,sizeof(s_vert2dc));
		g_device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);

		g_device->SetFVF(VERT2DT);
		RenderTextures();
		g_device->SetFVF(VERT2DC);

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

		if (attackpanel){
			g_device->SetStreamSource(0,ap_vb,0,sizeof(s_vert2dc));
			g_device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
			g_device->DrawPrimitive(D3DPT_LINESTRIP,4,4);
			font->DrawText(NULL,attacktitle,-1,&fontap, DT_TOP | DT_CENTER | DT_NOCLIP,0xffffffff);
			if (resultpanel){
				g_device->DrawPrimitive(D3DPT_LINESTRIP,19,4);
				font->DrawText(NULL,"Close",-1,&fontap, DT_BOTTOM | DT_CENTER | DT_NOCLIP,0xffffffff);
			} else {
				g_device->DrawPrimitive(D3DPT_LINESTRIP,9,4);
				g_device->DrawPrimitive(D3DPT_LINESTRIP,14,4);
				for (int i=g_nbSelected-1; i>=0; --i){
					font->DrawText(NULL,attackstr[i][0],-1,&fontap, DT_TOP | DT_LEFT | DT_NOCLIP,0xffffffff);
					font->DrawText(NULL,attackstr[i][1],-1,&fontap, DT_TOP | DT_RIGHT | DT_NOCLIP,0xffffffff);
				}
				font->DrawText(NULL,attackrez,-1,&fontap, DT_TOP | DT_LEFT | DT_NOCLIP,0xffffffff);
				font->DrawText(NULL,"Cancel           Attack",-1,&fontap, DT_BOTTOM | DT_CENTER | DT_NOCLIP,0xffffffff);
			}
		}

		RenderText();
	}
	frame_counter++;
}
//---------------------------------------------------------
void Hud::Toggle(){
	show = !show;
}
