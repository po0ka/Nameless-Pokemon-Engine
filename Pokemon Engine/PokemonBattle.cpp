#include "stdafx.h"
#include "PokemonBattle.h"
#include "partyMenu.h"
#include "text.h"

extern BattleEngineGraphics *BattleUIGFX;
extern TTF_Font *gFont;
extern SDL_Renderer *gRenderer;
extern int battleScene;
extern bool pressingEnter;

extern PokemonBattle *m_Battle;

void MoveCursorMenu0( const Uint8 *keystate, BattleMenu *menu, SDL_Event events );
int MoveCursorMenu1( const Uint8 *keystate, BattleMenu *menu, SDL_Event events );

void PokemonBattle::Initialise( Pokemon *battler1, Pokemon *battler2, bool isWild, Trainer *m_Trainer )
{
	m_pkmBattler1 = battler1;
	m_pkmBattler2 = battler2;

	m_pkmBattler1->side = 1;
	m_pkmBattler2->side = 0;

	BattleUIGFX->hpDisp->SetPokes( m_pkmBattler1, m_pkmBattler2 );

	BattleUIGFX->menu->SetCurrentPokemon( m_pkmBattler2 );

	BattleUIGFX->AddPoke(m_pkmBattler1, 0);
	BattleUIGFX->AddPoke(m_pkmBattler2, 1);

	m_bWild = isWild;
	m_trTrainer = m_Trainer;
};

bool PokemonBattle::Tick()
{
	if (SDL_PollEvent(&events))
	{
		if (events.type == SDL_QUIT)
		{
			return false;
		}
	}

	//Keyboard:
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	//Uint8 *mousestate = SDL_GetMouseState(NULL, NULL);

	if( BattleUIGFX->menu->subMenu == 0 )
	{
		MoveCursorMenu0( keystate, BattleUIGFX->menu, events );
	}
	else if( BattleUIGFX->menu->subMenu == 1 )
	{
		int attack = MoveCursorMenu1( keystate, BattleUIGFX->menu, events );
		attack--;

		if( attack >= 0 && attack <= 3 )
		{
			if( m_pkmBattler2->pAttacks[attack]->GetID() == 0 )
				goto renderFuncs;

			//Todo: Move priority
			if( m_pkmBattler2->GetStat( "speed" ) > m_pkmBattler1->GetStat( "speed" ) ) //If defender outspeeds attacker:
			{
				m_pkmBattler2->Attack( m_pkmBattler1, attack );
				m_pkmBattler1->Attack( m_pkmBattler2, rand()%4, true ); //Todo: add better AI!
			}
			else if( m_pkmBattler2->GetStat( "speed" ) < m_pkmBattler1->GetStat( "speed" ) ) //If attacker outspeeds defender:
			{
				m_pkmBattler1->Attack( m_pkmBattler2, rand()%4, true ); //Todo: add better AI!
				m_pkmBattler2->Attack( m_pkmBattler1, attack );
			}
			else if( rand()%2 == 0 )
			{
				m_pkmBattler2->Attack( m_pkmBattler1, attack );
				m_pkmBattler1->Attack( m_pkmBattler2, rand()%4, true ); //Todo: add better AI!
			}
			else
			{
				m_pkmBattler1->Attack( m_pkmBattler2, rand()%4, true ); //Todo: add better AI!
				m_pkmBattler2->Attack( m_pkmBattler1, attack );
			}
			BattleUIGFX->menu->cursorPos = 1;
			BattleUIGFX->menu->subMenu = 0;
		}
	}

	renderFuncs:

	SDL_RenderClear( gRenderer );

	BattleUIGFX->bg->Render();
	m_pkmBattler1->Render( gRenderer );
	m_pkmBattler2->Render( gRenderer );

	BattleUIGFX->menu->Render();

	BattleUIGFX->hpDisp->UpdateHP( m_pkmBattler2->GetHealth(), m_pkmBattler1->GetHealth(), m_pkmBattler2->GetStat( "hp" ), m_pkmBattler1->GetStat( "hp" ) );
	BattleUIGFX->hpDisp->Render();

	SDL_RenderPresent( gRenderer );

	return true;
}

extern PokemonPartyScene *m_Party;

void MoveCursorMenu0( const Uint8 *keystate, BattleMenu *menu, SDL_Event events )
{
	if ( keystate[SDL_GetScancodeFromKey(SDLK_w)] || keystate[SDL_GetScancodeFromKey(SDLK_UP)] )
	{
		if( menu->cursorPos == 3 )
		{
			menu->cursorPos = 1;
		}
		if( menu->cursorPos == 4 )
		{
			menu->cursorPos = 2;
		}
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_s)] || keystate[SDL_GetScancodeFromKey(SDLK_DOWN)] )
	{
		if( menu->cursorPos == 1 )
		{
			menu->cursorPos = 3;
		}
		if( menu->cursorPos == 2 )
		{
			menu->cursorPos = 4;
		}
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_d)] || keystate[SDL_GetScancodeFromKey(SDLK_RIGHT)] )
	{
		if( menu->cursorPos == 1 )
		{
			menu->cursorPos = 2;
		}
		if( menu->cursorPos == 3 )
		{
			menu->cursorPos = 4;
		}
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_a)] || keystate[SDL_GetScancodeFromKey(SDLK_LEFT)])
	{
		if( menu->cursorPos == 2 )
		{
			menu->cursorPos = 1;
		}
		if( menu->cursorPos == 4 )
		{
			menu->cursorPos = 3;
		}
	}

	if( pressingEnter )
	{
		if( menu->cursorPos == 1 )
		{
			menu->subMenu = 1;
		}
		if( menu->cursorPos == 3 )
		{
			//FadeToBlack(  );
			battleScene = SCENE_PARTY;
			m_Party->m_iSelection = 0;

			pressingEnter = false;
		}
		if( menu->cursorPos == 4 )
		{
			menu->subMenu = -1;
			BattleText( "Got away safely!", gRenderer, BattleUIGFX, gFont );
			menu->cursorPos = 1;
			menu->subMenu = 0;

			FadeToBlack(  );
			battleScene = SCENE_OVERWORLD;

			m_Battle->Clear();
		}

		pressingEnter = false;
	}
}

int MoveCursorMenu1( const Uint8 *keystate, BattleMenu *menu, SDL_Event events )
{
	if ( keystate[SDL_GetScancodeFromKey(SDLK_w)] || keystate[SDL_GetScancodeFromKey(SDLK_UP)])
	{
		if( menu->cursorPos == 3 )
		{
			menu->cursorPos = 1;
		}
		if( menu->cursorPos == 4 )
		{
			menu->cursorPos = 2;
		}
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_s)] || keystate[SDL_GetScancodeFromKey(SDLK_DOWN)])
	{
		if( menu->cursorPos == 1 )
		{
			menu->cursorPos = 3;
		}
		if( menu->cursorPos == 2 )
		{
			menu->cursorPos = 4;
		}
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_d)] || keystate[SDL_GetScancodeFromKey(SDLK_RIGHT)])
	{
		if( menu->cursorPos == 1 )
		{
			menu->cursorPos = 2;
		}
		if( menu->cursorPos == 3 )
		{
			menu->cursorPos = 4;
		}
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_a)] || keystate[SDL_GetScancodeFromKey(SDLK_LEFT)] )
	{
		if( menu->cursorPos == 2 )
		{
			menu->cursorPos = 1;
		}
		if( menu->cursorPos == 4 )
		{
			menu->cursorPos = 3;
		}
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_ESCAPE)] )
	{
		menu->subMenu = 0;
	}
	if( pressingEnter )
	{
		pressingEnter = false;
		return menu->cursorPos;
	}

	return 0;
}

void PokemonBattle::SwapOut( Pokemon *NewBattler, int side )
{
	//Todo: Add animation!

	std::string WithdrawText;

	switch( side )
	{
		case 0: m_pkmBattler1 = NewBattler; break;
		case 1: 
			BattleUIGFX->menu->subMenu = -1;
			WithdrawText = "PLAYER_NAME withdrew ";
			WithdrawText += m_pkmBattler2->m_sPkmName;
			BattleText( WithdrawText, gRenderer, BattleUIGFX, gFont );

			WithdrawText = "PLAYER_NAME sent out ";
			WithdrawText += NewBattler->m_sPkmName;
			BattleText( WithdrawText, gRenderer, BattleUIGFX, gFont );

			m_pkmBattler2 = NewBattler;
			break;
	}

	//Refresh UI elements!
	BattleUIGFX->hpDisp->SetPokes( m_pkmBattler1, m_pkmBattler2 );

	BattleUIGFX->menu->SetCurrentPokemon( m_pkmBattler2 );

	BattleUIGFX->hpDisp->UpdateHP( m_pkmBattler2->GetHealth(), m_pkmBattler1->GetHealth(), m_pkmBattler2->GetStat( "hp" ), m_pkmBattler1->GetStat( "hp" ) );

	BattleUIGFX->AddPoke(m_pkmBattler1, 0);
	BattleUIGFX->AddPoke(m_pkmBattler2, 1);

	if( side == 1 )
	{
		m_pkmBattler1->Attack( m_pkmBattler2, rand()%4, true );
		BattleUIGFX->menu->cursorPos = 1;
		BattleUIGFX->menu->subMenu = 0;
	}

	pressingEnter = false;
}

void PokemonBattle::WildBattleStartAnim()
{
	//Fade in:
	BattleUIGFX->menu->subMenu = -1;

	int progress = 255;
	while( true )
	{
		SDL_Surface *surf = SDL_CreateRGBSurface( SDL_SWSURFACE, 600, 480, 1, 0,0,0, progress );
		SDL_Texture *texture = SDL_CreateTextureFromSurface( gRenderer, surf );

		SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_BLEND );
		SDL_SetTextureAlphaMod( texture, progress );
		SDL_SetTextureColorMod( texture, 0, 0, 0 );

		SDL_RenderClear(gRenderer);
		BattleUIGFX->bg->Render();
		BattleUIGFX->menu->Render();
		m_pkmBattler1->Render( gRenderer );

		SDL_RenderCopy( gRenderer, texture, NULL, NULL );

		SDL_RenderPresent( gRenderer );

		progress-=5;

		if( progress <= 0 )
		{
			break;
		}

		SDL_Delay( 1 );
	}

	BattleUIGFX->bg->Render();
	m_pkmBattler1->Render( gRenderer );
	BattleUIGFX->menu->Render();

	m_pkmBattler2->m_bShouldRender = false;

	std::string pokeText = "";
	pokeText = "A wild " + m_pkmBattler1->GetName() + " Appeared!";
	BattleText( pokeText, gRenderer, BattleUIGFX, gFont, false );

	//Move trainter sprite:
	pokeText = "Go " + m_pkmBattler2->GetName() + "!";
	BattleText( pokeText, gRenderer, BattleUIGFX, gFont, false );

	//Spawn player pokemon:

	m_pkmBattler2->m_bShouldRender = true;

	//Done!
	BattleUIGFX->menu->subMenu = 0;
}