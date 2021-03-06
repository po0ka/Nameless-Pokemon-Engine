#include "stdafx.h"
#include "partyMenu.h"
#include "text.h"
#include "PokemonBattle.h"
#include "PokemonSummaryScreen.h"

extern SDL_Renderer *gRenderer;
extern TTF_Font *gFont;
extern int battleScene;
extern PokemonBattle *m_Battle;
extern PokemonSummaryScene *m_summary;

extern bool pressingEnter;

void PokemonPartyScene::Initialise( Player *player )
{
	m_Player = player;

	//Textures:
	SDL_Surface* loadedSurface = IMG_Load( "DATA/GFX/UI/party.png" );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", "DATA/GFX/UI/party.png", IMG_GetError() );
	}

	SDL_Surface*  loadedSurface_2=  IMG_Load( "DATA/GFX/UI/GBAMenuBars.png" );

	if( loadedSurface_2 == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", "DATA/GFX/UI/GBAMenuBars.png", IMG_GetError() );
	}

	m_Texture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

	mSelectorTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface_2 );
}

bool PokemonPartyScene::Tick()
{
	//Renderer:

	SDL_RenderClear( gRenderer );

	RenderBG();
	RenderMain();
	RenderOthers();

	SDL_RenderPresent( gRenderer );

	//Main handler:
	if (SDL_PollEvent(&events))
	{
		if (events.type == SDL_QUIT)
		{
			return false;
		}
		if (events.type == SDL_KEYDOWN)
		{
			if( events.key.keysym.sym == SDLK_s || events.key.keysym.sym == SDLK_DOWN )
			{
				if( !m_bHasSelected )
				{
					m_iSelection ++;
					if( m_Player->m_pkmParty[m_iSelection] == NULL )
					{
						m_iSelection = 6;
					}
					if( m_iSelection > 6 )
						m_iSelection = 0;
				}
				else
				{
					m_iPkmnSelection ++;
					if( m_iPkmnSelection > 2 ) //Fixme: Tempory
						m_iPkmnSelection = 0;
				}
			}
			if( events.key.keysym.sym == SDLK_w | events.key.keysym.sym == SDLK_UP )
			{
				if( !m_bHasSelected )
				{
					m_iSelection --;

					if( m_Player->m_pkmParty[m_iSelection] == NULL )
					{
						for( int i = 0; i < 6; i++ )
						{
							if( m_Player->m_pkmParty[i] != NULL )
							{
								m_iSelection = i;
							}
						}
					}

					if( m_iSelection < 0 )
						m_iSelection = 6;
				}
				else
				{
					m_iPkmnSelection --;
					if( m_iPkmnSelection < 0 ) //Fixme: Tempory
						m_iPkmnSelection = 2;
				}
			}

			if( events.key.repeat > 0 )
				return true;

			//Esc key!
			if( events.key.keysym.sym == SDLK_ESCAPE && !pressingEnter )
			{
				if( m_bHasSelected )
				{
					m_bHasSelected = false;
				}
				else
				{
					if( m_iSelection == 6 )
					{
						m_iSelection = 0; //Reset for next call
						m_bHasSelected = false;
						m_iPkmnSelection = 0;
						battleScene = SCENE_BATTLE;
					}
					m_iSelection = 6;
				}
			}
		}
	}

	if( pressingEnter )
	{
		HandleSelection();
	}

	return true;
}

void PokemonPartyScene::RenderBG()
{
	//Render to screen
	SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 5, 5, 240, 160 ), &GetRect( 0, 0, 600, 480 ) );

	SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 250, 5, 240, 128 ), &GetRect( 0, 0, 600, 352 ) );
}

void PokemonPartyScene::RenderMain()
{
	if( m_iSelection == 0 )
	{
		SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 405, 170, 85, 57 ), &GetRect( 2*2.5, 12*3, 85*2.5, 57*3 ) );
	}
	else
	{
		SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 317, 172, 85, 55 ), &GetRect( 2*2.5 + 3.5, 12*3 + 6, 85*2.5, 55*3 ) );
	}

	CText *pokeName = new CText( m_Player->m_pkmParty[0]->m_sPkmName , gRenderer, gFont, 1, 255, 255, 255 );
	pokeName->Render( &GetRect( 35*2.5, 31*3, 0, 0 ) );

	pokeName = new CText( std::to_string( (_ULonglong)m_Player->m_pkmParty[0]->m_iLevel) , gRenderer, gFont, 1, 255, 255, 255 );
	pokeName->Render( &GetRect( 50*2.5, 42.5*3, 0, 0 ) );

	//HP bar:
	int TargetHPBarWidth;
	float Hp, MaxHp;
	Hp = m_Player->m_pkmParty[0]->GetHealth();
	MaxHp = m_Player->m_pkmParty[0]->GetStat( "hp" );

	int colourBar1Red, colourBar1Green, colourBar1Blue;
	int colourBar2Red, colourBar2Green, colourBar2Blue;

	TargetHPBarWidth = (Hp/MaxHp) * 120;

	if( Hp > (MaxHp/3)*2 )
	{
		colourBar1Red = 122; colourBar1Green = 248, colourBar1Blue = 168;
		colourBar2Red = 88; colourBar2Green = 208, colourBar2Blue = 128;
	}
	if( Hp < (MaxHp/3)*2 )
	{
		colourBar1Red = 248; colourBar1Green = 224, colourBar1Blue = 56;
		colourBar2Red = 200; colourBar2Green = 168, colourBar2Blue = 8;
	}
	if( Hp < (MaxHp/3) )
	{
		colourBar1Red = 248; colourBar1Green = 88, colourBar1Blue = 56;
		colourBar2Red = 168; colourBar2Green = 64, colourBar2Blue = 72;
	}

	SDL_SetRenderDrawColor( gRenderer, colourBar1Red, colourBar1Green, colourBar1Blue, 255);
	SDL_RenderFillRect( gRenderer, &GetRect( 33*2.5, 53*3, TargetHPBarWidth, 10 ) );
	SDL_SetRenderDrawColor( gRenderer, colourBar2Red, colourBar2Green, colourBar2Blue, 255);
	SDL_RenderFillRect( gRenderer, &GetRect( 33*2.5, 53*3, TargetHPBarWidth, 3 ) );
}

void PokemonPartyScene::RenderOthers()
{
	int yOffset;

	yOffset = 9;

	for( int i = 1; i < 6; i++ )
	{
		if( m_Player->m_pkmParty[i] != NULL )
		{
			if( m_iSelection == i )
			{
				SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 162, 203, 150, 24 ), &GetRect( 90*2.5, yOffset*3 - 2, 148*2.5, 20*3 + 4 ) );
			}
			else
			{
				SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 162, 179, 150, 22 ), &GetRect( 90*2.5, yOffset*3, 148*2.5, 20*3 ) );
			}

			yOffset += 4;

			CText *pokeName = new CText( m_Player->m_pkmParty[i]->m_sPkmName , gRenderer, gFont, 1, 255, 255, 255 );
			pokeName->Render( &GetRect( 117*2.5, yOffset*3, 0, 0 ) );

			//HP bar:
			int TargetHPBarWidth;
			float Hp, MaxHp;
			Hp = m_Player->m_pkmParty[i]->GetHealth();
			MaxHp = m_Player->m_pkmParty[i]->GetStat( "hp" );

			int colourBar1Red, colourBar1Green, colourBar1Blue;
			int colourBar2Red, colourBar2Green, colourBar2Blue;

			TargetHPBarWidth = (Hp/MaxHp) * 119;

			if( Hp > (MaxHp/3)*2 )
			{
				colourBar1Red = 122; colourBar1Green = 248, colourBar1Blue = 168;
				colourBar2Red = 88; colourBar2Green = 208, colourBar2Blue = 128;
			}
			if( Hp < (MaxHp/3)*2 )
			{
				colourBar1Red = 248; colourBar1Green = 224, colourBar1Blue = 56;
				colourBar2Red = 200; colourBar2Green = 168, colourBar2Blue = 8;
			}
			if( Hp < (MaxHp/3) )
			{
				colourBar1Red = 248; colourBar1Green = 88, colourBar1Blue = 56;
				colourBar2Red = 168; colourBar2Green = 64, colourBar2Blue = 72;
			}

			yOffset += 3;

			SDL_SetRenderDrawColor( gRenderer, colourBar1Red, colourBar1Green, colourBar1Blue, 255);
			SDL_RenderFillRect( gRenderer, &GetRect( 185*2.5, yOffset*3, TargetHPBarWidth, 10 ) );
			SDL_SetRenderDrawColor( gRenderer, colourBar2Red, colourBar2Green, colourBar2Blue, 255);
			SDL_RenderFillRect( gRenderer, &GetRect( 185*2.5, yOffset*3, TargetHPBarWidth, 3 ) );

			yOffset += 5.5;

			pokeName = new CText( std::to_string( (_ULonglong)m_Player->m_pkmParty[i]->m_iLevel) , gRenderer, gFont, 1, 255, 255, 255 );
			pokeName->Render( &GetRect( 137*2.5, yOffset*3, 0, 0 ) );
		}
		yOffset = (9 + (23 * i)) - i;
	}


	//Handle cancel and textbox:

	if( m_bHasSelected == false )
	{
		SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 162, 232, 180, 28 ), &GetRect( 5, 357 , 180*2, 120  ) );

		CText *txt = new CText( "Chose a Pok�mon.", gRenderer, gFont, 1);
		txt->Render( &GetRect( 20, 407 , 0, 0  ) );

		if( m_iSelection == 6 )
		{
			SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 65, 250, 54, 25 ), &GetRect( 380, 357, 180, 120  ) );
		}
		else
		{
			SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 6, 250, 54, 25 ), &GetRect( 380, 357, 180, 120  ) );
		}
	}
	else
	{
		SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 162, 232, 180, 28 ), &GetRect( 5, 357 , 360, 120  ) );
		SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 162, 232, 180, 28 ), &GetRect( 370, 300, 200, 177 ) );

		CText *txt = new CText( "Do what with this PKMN?", gRenderer, gFont, 1);
		txt->Render( &GetRect( 20, 407 , 0, 0  ) );

		//Fixme: Tempory!
		txt = new CText( "SHIFT", gRenderer, gFont, 1);
		txt->Render( &GetRect( 420, 340, 0, 0  ) );

		txt = new CText( "SUMMARY", gRenderer, gFont, 1);
		txt->Render( &GetRect( 420, 380, 0, 0  ) );

		txt = new CText( "CANCEL", gRenderer, gFont, 1);
		txt->Render( &GetRect( 420, 420, 0, 0  ) );

		SDL_RenderCopy( gRenderer, mSelectorTexture, &GetRect( 269, 4, 6, 10 ), &GetRect( 395, 340 + (m_iPkmnSelection*40), 20, 20 ) );
	}
}

void PokemonPartyScene::HandleSelection()
{
	if( m_bHasSelected == false )
	{
		//Cancel?
		if( m_iSelection == 6 )
		{
			m_iSelection = 0; //Reset for next call
			m_bHasSelected = false;
			m_iPkmnSelection = 0;
			battleScene = SCENE_BATTLE;

			return;
		}

		if( m_Player->m_pkmParty[m_iSelection] != NULL )
		{
			m_bHasSelected = true;
		}
	}
	else
	{
		if( m_iPkmnSelection == 2 )
		{
			m_bHasSelected = false;
		}
		if( m_iPkmnSelection == 1 )
		{
			//Add summary screen here!
			m_summary->m_iSelection = m_iSelection;
			battleScene = SCENE_SUMMARY;
			return;
		}
		if( m_iPkmnSelection == 0 )
		{
			//Here goes nothing...
			//Cant send in myself!
			if( m_iSelection == 0 )
			{
				//Todo: Add stuff here!
				return;
			}

			Pokemon *pokeBuffer;
			pokeBuffer = m_Player->m_pkmParty[m_iSelection];

			m_Player->m_pkmParty[m_iSelection] = m_Player->m_pkmParty[0];
			m_Player->m_pkmParty[0] = pokeBuffer;

			//Reset scene!
			m_bHasSelected = false;
			m_iPkmnSelection = 0;
			m_iSelection = 0;
			battleScene = SCENE_BATTLE;

			m_Battle->SwapOut( m_Player->m_pkmParty[0], 1 );
		}
	}
}
