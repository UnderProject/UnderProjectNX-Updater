/*
* ReiNX Toolkit
* Copyright (C) 2018  Team ReiSwitched
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <ctime>
#include <switch.h>
#include <fstream>
#include <string>
#include "Net/Request.hpp"
#include "Utils/unzip_utils.hpp"
#include "FS.hpp"
#include "UI.hpp"
#include "Power.hpp"
#include "kernel.h"

#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#define WIN_WIDTH 1280
#define WIN_HEIGHT 720
using namespace std;


static u32 currSel = 0;
static u32 currSubSel = 0;
static u32 vol = 64;
static u32 titleX = 60;
static u32 titleY = 30;
static u32 menuX = 55, menuY = 115;
static u32 subX = 411, subY = 88;

// Input vars, updated every iteration of the loop, proper way to access input using CustomUI
static u64 HeldInput = 0;
static u64 PressedInput = 0;
static u64 ReleasedInput = 0;



static string title;
static string version;
string origen;
string destino;
string new_release = "";

string StarDust_Autoboot = "";
string HBnew_release = "";
string vernx;
string cfwpath = "atmosphere";
/*
customisation
*/
//unerpnx last release
string change_upd_url = "http://cloud.not-d3fau4.tk/UPNX.php";
//php with the url for download
string GetPatch = "";


u32 dev_count = 1;
u32 render_count = 0;
u32 rig_count = 1;
u32 clippy = 0;
u32 about = 0;
u32 keystrick = 0;
vector<MenuOption> mainMenu;
vector<SDL_Surface*> images;
Mix_Music *menuSel;
Mix_Music *menuConfirm;
Mix_Music *menuBack;

UI * UI::mInstance = 0;





/**
Thanks to PricelessTwo2
**/
//Ver *.*.*-** 

string SwitchIdent_GetFirmwareVersion(void) {
    setsysInitialize();
	Result ret = 0;
    SetSysFirmwareVersion ver;
    if (R_FAILED(ret = setsysGetFirmwareVersion(&ver))) {
        printf("setsysGetFirmwareVersion() failed: 0x%x.\n\n", ret);
		std:string mosca = "NO VA a joderse";
		return mosca;
    }
    static char buf[9];
    snprintf(buf, 19, "%u.%u.%u-%u%u", ver.major, ver.minor, ver.micro, ver.revision_major, ver.revision_minor);
        std::string switchver = std::string(buf);
		setsysExit();
    return switchver;
}

/*
* copy function
*/
bool copy_me(string origen, string destino) {
    clock_t start, end;
    start = clock();
    ifstream source("sdmc:"+origen+"", ios::binary);
    ofstream dest("sdmc:"+destino+"", ios::binary);

    dest << source.rdbuf();

    source.close();
    dest.close();

    end = clock();

    cout << "CLOCKS_PER_SEC " << CLOCKS_PER_SEC << "\n";
    cout << "CPU-TIME START " << start << "\n";
    cout << "CPU-TIME END " << end << "\n";
    cout << "CPU-TIME END - START " <<  end - start << "\n";
    cout << "TIME(SEC) " << static_cast<double>(end - start) / CLOCKS_PER_SEC << "\n";
	return 0;
}

/* ---------------------------------------------------------------------------------------
* Menu functions
*/
//cfw select
void UI::optautobootatms() {
	//atm
	cfwpath = "atmosphere";
	optGetPatch();
}
void UI::optautobootrei() {
	//rei
	cfwpath = "ReiNX";
	optGetPatch();
}
void UI::optautobootsxos() {
	//sxos
	cfwpath = "sxos";
	optGetPatch();
}

//UnderProjectNX Selecciona tu CFW
void UI::optGetPatch() {
    ProgBar prog;
    prog.max = 4;
    prog.step = 1;
	string filename = "/UPNX.zip";
    Net net = Net();
    hidScanInput();
	string url_down;
	url_down = "http://cloud.not-d3fau4.tk/nextcloud/public.php/webdav";
    
	CreateProgressBar(&prog, "Get UnderProjectNX...");
    bool res = net.Download(url_down,filename );
    IncrementProgressBar(&prog);
    if(!res){
        appletBeginBlockingHomeButton(0);
        unzFile zip = Utils::zip_open(filename.c_str()); IncrementProgressBar(&prog);
		if(cfwpath == "atmosphere"){Utils::zip_extract_all(zip, "/atmosphere/"); IncrementProgressBar(&prog);}
		if(cfwpath == "ReiNX"){Utils::zip_extract_all(zip, "/ReiNX/"); IncrementProgressBar(&prog);}
		if(cfwpath == "sxos"){Utils::zip_extract_all(zip, "/sxos/"); IncrementProgressBar(&prog);}
        Utils::zip_close(zip); IncrementProgressBar(&prog);
        remove(filename.c_str());
        appletEndBlockingHomeButton();
		remove("/switch/UPNXver.txt");
		string secconder = "Ultima version instalada en "+cfwpath;
		std::ofstream notes("sdmc:/switch/UPNXver.txt", std::ios::app);
		notes << secconder;
		notes.close();
		rig_count = 1;
		MessageBox("Patch","Patch Apply successfully!-.-", TYPE_OK);
    }else{
		MessageBox("Patch","error"+GetPatch+"!-.-", TYPE_OK);
        return;
    }
}

//Update ME
void UI::optUpdateHB() {
    ProgBar prog;
    prog.max = 1;
    prog.step = 1;
    string url = "";
	

	url = "http://cloud.not-d3fau4.tk/nextcloud/public.php/webdav";
    CreateProgressBar(&prog, "Updating UnderProject-Updater...");
    hidScanInput();

    if (!MessageBox("Update", 
      "This will attempt to update the Toolkit.\nAfter updating, the app will exit.\n\nContinue?", 
      TYPE_YES_NO))
        return;
    Net net = Net();
    if (net.Download1(url, "/switch/UnderProject-Updater_new.nro")){
        prog.curr = 1;
        appletEndBlockingHomeButton();
        MessageBox("Update", "Update unsuccessful!", TYPE_OK);
        return;
    }
    IncrementProgressBar(&prog);
    romfsExit();
    remove("/switch/UnderProject-Updater.nro");
    rename("/switch/UnderProject-Updater_new.nro", "/switch/UnderProject-Updater.nro");
    fsdevCommitDevice("sdmc");
    exitApp();

	}
}

//about
void UI::optAbout() {
		MessageBox(
        "About", 
        "Version: " + version +
		"\nFirmware Ver :"+vernx+"\n"+
        "Desarrollador Reisyukaku\n" +
		"Modificado por Kronos2308\n" +
        "Para UnderProject Team\n" +
        "" ,    TYPE_OK);
	//Easter egg ;^)
		about++;
    if (about == 3) {
	    mainMenu.push_back(MenuOption("The truth", "Get the Truth now.",  bind(&UI::optTheTruth, this)));
		SDL_RenderPresent(mRender._renderer);
	}
}
void UI::optTheTruth() {
appletBeginBlockingHomeButton(0);
SDL_Surface* image1 = IMG_Load("romfs:/Graphics/ForReiNXLight.png");
SDL_Texture* tex1 = SDL_CreateTextureFromSurface(mRender._renderer, image1);
drawBackXY(image1, tex1, 0, 0);
SDL_RenderPresent(mRender._renderer);
Mix_PlayMusic(menuSel, 1);
SDL_Delay(200);
Mix_PlayMusic(menuConfirm, 1);
SDL_Delay(200);
Mix_PlayMusic(menuBack, 1);
SDL_Delay(1000);
SDL_DestroyTexture(tex1);


exitApp();
}

/* ---------------------------------------------------------------------------------------
* UI class functions
*/
UI::UI(string Title, string Version) {
    romfsInit();
    fsdevMountSdmc();
    socketInitializeDefault();
#ifdef DEBUG
    nxlinkStdio();
    printf("printf output now goes to nxlink server\n");
#endif
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(WIN_WIDTH, WIN_HEIGHT, 0, &mRender._window, &mRender._renderer);
    mRender._surface = SDL_GetWindowSurface(mRender._window);
    SDL_SetRenderDrawBlendMode(mRender._renderer, SDL_BLENDMODE_BLEND);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    TTF_Init();
    SDL_SetRenderDrawColor(mRender._renderer, 255, 255, 255, 255);
    Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
    Mix_VolumeMusic(vol);
    mThemes = Themes::instance();
    mThemes->Init(mRender);
    inSubMenu = false;

    title = Title;
    version = Version;
    menuSel = Mix_LoadMUS("romfs:/Sounds/menu_select.mp3");
    menuConfirm = Mix_LoadMUS("romfs:/Sounds/menu_confirm.mp3");
    menuBack = Mix_LoadMUS("romfs:/Sounds/menu_back.mp3");
   
    //Main pages
    mainMenu.push_back(MenuOption("UnderProjectNX", "Selecciona tu CFW.", nullptr));
	mainMenu.push_back(MenuOption("DeltaProjectNX", "Selecciona tu CFW.", nullptr));
    mainMenu.push_back(MenuOption("tools", "tools.", nullptr));
    mainMenu.push_back(MenuOption("About", "About UnderProjectNX Updater.",  bind(&UI::optAbout, this)));

    //Subpages

    mainMenu[0].subMenu.push_back(MenuOption("Atmosphere", "", bind(&UI::optautobootatms, this)));
    mainMenu[0].subMenu.push_back(MenuOption("ReiNX", "", bind(&UI::optautobootrei, this)));
    mainMenu[0].subMenu.push_back(MenuOption("SXOS", "", bind(&UI::optautobootsxos, this)));
    mainMenu[3].subMenu.push_back(MenuOption("Update ME", "", bind(&UI::optUpdateHB, this)));
	mainMenu[2].subMenu.push_back(MenuOption("Atmosphere", "", bind(&UI::optautobootatms, this)));
    mainMenu[2].subMenu.push_back(MenuOption("ReiNX", "", bind(&UI::optautobootrei, this)));
    mainMenu[2].subMenu.push_back(MenuOption("SXOS", "", bind(&UI::optautobootsxos, this)));
	
//	mainMenu[0].subMenu.push_back(MenuOption("Borrar-parche, "", bind(&UI::optautobootdes, this)));
vernx = SwitchIdent_GetFirmwareVersion();
	 Net net = Net();
	new_release = net.Request("GET",change_upd_url);
	new_release = net.readBuffer;
	
}
void UI::setInstance(UI ui) {
    mInstance = &ui;
}

UI *UI::getInstance() {
    if(!mInstance)
        return NULL;
    return mInstance;
}

void UI::deinit() {
    TTF_Quit();
    IMG_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_DestroyRenderer(mRender._renderer);
    SDL_FreeSurface(mRender._surface);
    SDL_DestroyWindow(mRender._window);
    SDL_Quit();
    romfsExit();
    socketExit();
    fsdevUnmountAll();
}

void UI::exitApp() {
    deinit();
    appletEndBlockingHomeButton(); // make sure we don't screw up hbmenu
    ::exit(0);
}

/*
* UI draw functions
*/
void UI::drawText(int x, int y, SDL_Color scolor, string text, TTF_Font *font) {
	SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), scolor, 1280);

	if (!surface) {
		return;
	}

	SDL_SetSurfaceAlphaMod(surface, 255);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(mRender._renderer, surface);

	if (texture) {
		SDL_Rect position;
		position.x = x;
		position.y = y;
		position.w = surface->w;
		position.h = surface->h;

		SDL_RenderCopy(mRender._renderer, texture, NULL, &position);
		SDL_DestroyTexture(texture);
	}
	SDL_FreeSurface(surface);
}


void UI::drawRect(int x, int y, int w, int h, SDL_Color scolor, unsigned border, SDL_Color bcolor) {
    drawRect(x-border, y-border, w+(2*border), h+(2*border), bcolor);
    drawRect(x, y, w, h, scolor);
}

void UI::drawRect(int x, int y, int w, int h, SDL_Color scolor) {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_SetRenderDrawColor(mRender._renderer, scolor.r, scolor.g, scolor.b, scolor.a);
    SDL_RenderFillRect(mRender._renderer, &rect);
}

void UI::drawBackXY(SDL_Surface *surf, SDL_Texture *tex, int x, int y) {
    SDL_Rect position;
    position.x = x;
    position.y = y;
    position.w = surf->w;
    position.h = surf->h;
    SDL_RenderCopy(mRender._renderer, tex, NULL, &position);
}

void UI::drawScaled(SDL_Surface *surf, SDL_Texture *tex, int x, int y, u32 w, u32 h) {
    SDL_Rect position;
    position.x = x;
    position.y = y;
    position.w = w;
    position.h = h;
    SDL_RenderCopy(mRender._renderer, tex, NULL, &position);
}

/*
* UI Pop-up stuff
*/
void UI::CreatePopupBox(u32 x, u32 y, u32 w, u32 h, string header) {
    drawRect(x-5, y-5, w+10, h+70, {0, 0, 0, 0xFF}); //BG box
    drawRect(x, y, w, 57, mThemes->popCol1); //Header
    drawRect(x, y+60, w, h, mThemes->popCol2); //Message
    drawText(x+15, y+15, mThemes->txtcolor, header, mThemes->fntMedium);
}

void UI::CreateProgressBar(ProgBar *prog, string header) {
    u32 poph = 100, popw = 300;
    u32 barh = 40, barw = 200;
    u32 startx = (WIN_WIDTH/2)-(popw/2), starty = (WIN_HEIGHT/2)-(poph/2);
    
    if(prog->max <= 0) prog->max = 100;
    if(prog->step <= 0) prog->step = 1;
    prog->curr = 0;
    prog->posx = startx;
    prog->posy = starty;
    CreatePopupBox(prog->posx, prog->posy, popw, poph, header);
    drawRect(prog->posx+(popw/2)-(barw/2), prog->posy+60+((poph-60)/2), barw, barh, {0, 0, 0, 0xFF}); //empty bar
    drawText(prog->posx+(popw/2), prog->posy+(poph/2)-(barh/2)+60, {0xFF,0xFF,0xFF,0xFF}, to_string((unsigned)((prog->curr/prog->max)*100)) + "%", mThemes->fntSmall);
    SDL_RenderPresent(mRender._renderer);
}

void UI::IncrementProgressBar(ProgBar *prog) {
    u32 poph = 100+2, popw = 300+2;
    u32 barh = 40-4, barw = 200-4;
    drawRect(prog->posx+(popw/2)-(barw/2), prog->posy+60+((poph-60)/2), barw, barh, {0, 0, 0, 0xFF}); //empty bar
    drawRect(prog->posx+(popw/2)-(barw/2), prog->posy+60+((poph-60)/2), (unsigned)(barw*(prog->curr/prog->max)), barh, {0, 0xFF, 0, 0xFF}); //green bar
    drawText(prog->posx+(popw/2), prog->posy+(poph/2)-(barh/2)+60, {0xFF,0xFF,0xFF,0xFF}, to_string((unsigned)((prog->curr/prog->max)*100)) + "%", mThemes->fntSmall);
    prog->curr += prog->step;
    SDL_RenderPresent(mRender._renderer);
}

bool UI::MessageBox(string header, string message, MessageType type) {
    bool ret = false;
    SDL_Rect rect;
    u32 poph = 300, popw = 450;
    u32 buth = 50, butw = 100;
    u32 startx = (WIN_WIDTH/2)-(popw/2), starty = (WIN_HEIGHT/2)-(poph/2);
    
    CreatePopupBox(startx, starty, popw, poph, header);
    drawText(startx+15, starty+75, mThemes->txtcolor, message, mThemes->fntMedium);
    
    switch(type){
        case TYPE_YES_NO:
            drawRect(startx+popw-butw-10, starty+poph, butw, buth, mThemes->popCol1, 3, {0, 0, 0, 0xFF}); //YES
            drawText(startx+popw-butw+12, starty+poph+12, mThemes->txtcolor, "\ue0e0 Yes", mThemes->fntMedium);
            drawRect(startx+popw-(2*(butw+5))-10, starty+poph, butw, buth, mThemes->popCol1, 3, {0, 0, 0, 0xFF}); //NO
            drawText(startx+popw-(2*(butw+5))+12, starty+poph+12, mThemes->txtcolor, "\ue0e1 No", mThemes->fntMedium);
			
            break;
        default:
        case TYPE_OK:
			
            drawRect(startx+popw-butw-10, starty+poph, butw, buth, mThemes->popCol1, 3, {0, 0, 0, 0xFF}); //OK
            drawText(startx+popw-butw+12, starty+poph+12, mThemes->txtcolor, "\ue0e0 OK", mThemes->fntMedium);
			
            break;
    }

			

    SDL_RenderPresent(mRender._renderer);
    while(1){
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        if(kDown & KEY_A) {
            ret = true;
            Mix_PlayMusic(menuConfirm, 1);
            break;
        }
        else if(kDown & KEY_B) {
            ret = (type == TYPE_OK);
            Mix_PlayMusic(menuBack, 1);
            break;
        }
    }
    return ret;
}


/*
* Render function
*/
void UI::renderMenu() {
    SDL_RenderClear(mRender._renderer);
    drawBackXY(mThemes->bgs, mThemes->bgt, 0, 0);
	if (rig_count == 1) {
    string autobootecho = "sdmc:/switch/UPNXver.txt";
    std::ifstream open_echo(autobootecho.c_str());
    if (open_echo.is_open()) {
        std::string line;
        getline(open_echo, StarDust_Autoboot);
        open_echo.close();
    }else{StarDust_Autoboot = "";}
	rig_count++;
	}
	

	drawText(1150, titleY, mThemes->txtcolor,"v"+version, mThemes->fntLarge);//vercion HB

    drawText(500, 570, mThemes->txtcolor,StarDust_Autoboot, mThemes->fntMedium); //Autoboot

    int oy = menuY;
    if(!mainMenu.empty()) for(unsigned int i = 0; i < mainMenu.size(); i++){
        //Mainmenu buttons
        if(i == currSel && !inSubMenu) {
            drawRect(menuX-10, oy-10, 210+20, 25+20, mThemes->butCol, 5, mThemes->butBorderCol);
            drawText(menuX, oy, mThemes->selcolor, mainMenu[i].getName(), mThemes->fntMedium);
        } else {
            drawText(menuX, oy, mThemes->txtcolor, mainMenu[i].getName(), mThemes->fntMedium);
        }
        
        if(i == currSel) {
            //Submenu text
            drawText(subX + 30, subY + 30, mThemes->txtcolor, mainMenu[i].getDesc(), mThemes->fntSmall);
            if(!mainMenu[i].subMenu.empty()) for(unsigned int j = 0; j < mainMenu[i].subMenu.size(); j++){
                //Submenu buttons
                if(j == currSubSel && inSubMenu) {
                    drawRect(subX + 10, subY + 20 + ((j+1)*50), 200+20, 40, mThemes->butCol, 3, mThemes->butBorderCol);
                    drawText(subX + 30, subY + 30 + ((j+1)*50), mThemes->selcolor, mainMenu[i].subMenu[j].getName(), mThemes->fntMedium);
                }else{
                    drawText(subX + 30, subY + 30 + ((j+1)*50), mThemes->txtcolor, mainMenu[i].subMenu[j].getName(), mThemes->fntMedium);
                }
                if(j == currSubSel && currSel == 3) {
                    SDL_Texture* tex = SDL_CreateTextureFromSurface(mRender._renderer, images[currSubSel]);
                    drawScaled(images[currSubSel], tex, 710, 120, images[currSubSel]->w/3, images[currSubSel]->h/3);
                }
            }
        }
        oy += 50;
    }
    SDL_RenderPresent(mRender._renderer);
}

/*
* Menu nav functions
*/
void UI::MenuUp() {
    Mix_PlayMusic(menuSel, 1);
    if(currSel > 0) currSel--;
    else currSel = mainMenu.size() - 1;
    currSubSel = 0;
}

void UI::MenuDown() {
    Mix_PlayMusic(menuSel, 1);
    if((unsigned int) currSel < mainMenu.size() - 1) currSel++;
    else currSel = 0;
    currSubSel = 0;
}

void UI::SubMenuUp() {
    u32 subSize = mainMenu[currSel].subMenu.size();
    if(subSize > 1) Mix_PlayMusic(menuSel, 1);
    if(currSubSel > 0) currSubSel--;
    else currSubSel = subSize - 1;
}

void UI::SubMenuDown() {
    u32 subSize = mainMenu[currSel].subMenu.size();
    if(subSize > 1) 
        Mix_PlayMusic(menuSel, 1);
    if((unsigned int) currSubSel < subSize - 1) 
        currSubSel++;
    else currSubSel = 0;
}

void UI::MenuSel() {
    Mix_PlayMusic(menuConfirm, 1);
    if(!inSubMenu){
        if(mainMenu[currSel].subMenu.empty()) 
            mainMenu[currSel].callFunc();
        else 
            inSubMenu = true;
    }else{
        mainMenu[currSel].subMenu[currSubSel].callFunc();
    }
}

void UI::MenuBack() {
    Mix_PlayMusic(menuBack, 1);
    if(inSubMenu)
        inSubMenu = false;
    else
        exitApp();
}
