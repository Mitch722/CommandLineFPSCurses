
#include<SDL.h>

struct keysPressed 
{   
    void init() {

        w = false;
        a = false;
        s = false;
        d = false;

        q = false;
        e = false;
    
        quit = false;
    }

    bool w;
    bool a;
    bool s;
    bool d;

    bool q;
    bool e;

    bool quit;
};

class EventHandler
{
private:
    /* data */
    // creates the SDL event
    SDL_Event event_;

    // keep track of which keys are pressed
    keysPressed keysPressed_;

public:
    EventHandler(/* args */);
    ~EventHandler();

    // to end SDL specific things
    void endProcess();

    // poll for keyboard events
    void pollEvent();

    // getters
    const keysPressed getKeysPressed() { return keysPressed_; } 

};

EventHandler::EventHandler(/* args */)
{
    /* Initialise SDL */
    if( SDL_Init( SDL_INIT_VIDEO ) < 0){
        std::cout << stderr <<  "Could not initialise SDL: " << SDL_GetError() << std::endl;
        exit( -1 );
    }

    /* Set a video mode */
    if( !SDL_SetVideoMode( 1, 1, 0, 0 ) ){
        std::cout << stderr <<  "Could not set video mode: " <<  SDL_GetError() << std::endl;
        SDL_Quit();
        exit( -1 );
    }

    /* Enable Unicode translation */
    SDL_EnableUNICODE( 1 );

    /* initialise keysPressed */
    keysPressed_.init();
}

EventHandler::~EventHandler()
{
    SDL_Quit();
}

void EventHandler::endProcess()
{
    SDL_Quit();
}

void EventHandler::pollEvent() 

{

    while( SDL_PollEvent( &event_ ) ){
        switch( event_.type ){
            /* Look for a keypress */
            case SDL_KEYDOWN:
                /* Check the SDLKey values and update the keysPressed struct */
                switch( event_.key.keysym.sym ){
                    case SDLK_w:
                        keysPressed_.w = true;
                        break;
                    case SDLK_a:
                        keysPressed_.a = true;
                        break;
                    case SDLK_s:
                        keysPressed_.s = true;
                        break;
                    case SDLK_d:
                        keysPressed_.d = true;
                        break;
                    case SDLK_q:
                        keysPressed_.q = true;
                        break;
                    case SDLK_e:
                        keysPressed_.e = true;
                        break;
                    case SDLK_ESCAPE:
                        keysPressed_.quit = true; 
                    default:
                        break;
                }
                break;
            //  check for cases when keys are lifted
            case SDL_KEYUP:
                switch( event_.key.keysym.sym ){
                    case SDLK_w:
                        keysPressed_.w = false;
                        break;
                    case SDLK_a:
                        keysPressed_.a = false;
                        break;
                    case SDLK_s:
                        keysPressed_.s = false;
                        break;
                    case SDLK_d:
                        keysPressed_.d = false;
                        break;
                    case SDLK_q:
                        keysPressed_.q = false;
                        break;
                    case SDLK_e:
                        keysPressed_.e = false;
                        break;
                    case SDLK_ESCAPE:
                        keysPressed_.quit = false; 
                    default:
                        break;
                }
                break;
            
            default:
                break;
        }
    }


}

