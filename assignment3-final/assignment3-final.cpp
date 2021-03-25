#define _CRT_SECURE_NO_WARNINGS
#include<bangtal.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

SceneID scene;
ObjectID game_board[16], original_board[16] ;
ObjectID startbutton;
int blank = 15;

bool game = false;

TimerID timer;
float timeValue = 0.02f;    //퍼즐이 섞일때 한칸 움직이는데 걸리는 시간 - 속도
int mixCount;

int indexToX(int i)
{
	return 335 + 150 * (i % 4);
}

int indexToY(int i)
{
	return  525 - 150 * (i / 4);
}

int game_index(ObjectID object) 
{
	for (int i = 0; i < 16; i++)
	{
		if (game_board[i] == object)  return i;
	}
	return -1;
}

void game_move(int i)
{
	ObjectID object = game_board[i];
	game_board[i] = game_board[blank];
	locateObject(game_board[i], scene, indexToX(i), indexToY(i));
	game_board[blank] = object;
	locateObject(game_board[blank], scene, indexToX(blank), indexToY(blank));
	
	blank = i;
}

void game_print()
{
	for (int i = 0; i < 16; i++)
	{
		printf("%d ", game_board[i]);
	}
	printf("\n");
}

bool possible_move(int i)
{
	if (i % 4 > 0 && blank == i - 1) return true;
	if (i % 4 < 3 && blank == i + 1) return true;
	if (i / 4 > 0 && blank == i - 4) return true;
	if (i / 4 < 3 && blank == i + 4) return true;
	
	return false;
}

int random_move()
{
	int i = -1;   //not determined
	while (i == -1)
	{
		switch(rand() % 4)
		{
		case 0: if (blank % 4 > 0) 
		{
			i = blank - 1;
			break;
		}
		case 1: if (blank % 4 < 3)
		{
			i = blank + 1;
			break;
		}
		case 2: if (blank / 4 > 0)
		{
			i = blank - 4;
			break;
		}
		case 3: if (blank / 4 < 3)
		{
			i = blank + 4;
			break;
		}
		}
	}

	return i;
}

bool game_end()  //게임이 종료되는 조건
{
	for (int i = 0; i < 16; i++)
	{
		if (game_board[i] != original_board[i])
		{
			return false;
		}
	}
	return true;
}

void game_start()
	{
		blank = 15;
		hideObject(game_board[blank]);

		mixCount = 10;        //섞는 횟수

		setTimer(timer, timeValue);
		startTimer(timer);

	}

void mouseCallback(ObjectID object, int x, int y, MouseAction action)
{
	if (game)
	{
		int i = game_index(object);
		if (possible_move(i))
		{
			game_move(i);

			if (game_end())
			{
				game = false;
				showObject(startbutton);
				showObject(game_board[blank]);

				showMessage("completed");
			}
			game_print();
		}
	}
	else
	{
		if (object == startbutton)
		{
			game_start();
		}
	}
}

void timerCallback(TimerID timer)
{
	game_move(random_move());

	mixCount--;
	if (mixCount > 0)
	{
		setTimer(timer, timeValue);
		startTimer(timer);
	}

	else 
	{
		game = true;
		hideObject(startbutton);
	}

}

ObjectID createObject(const char* image, SceneID scene, int x, int y, bool shown = true)
{
	ObjectID object = createObject(image);
	locateObject(object, scene, x, y);
	if (shown) 
	{ 
		showObject(object); 
	}

	return object;
}

SceneID game_init()
{
	scene = createScene("시작화면", "뭉크의 절규 - 배경.png");

	char path[50];
	for (int i = 0; i < 16; i++)
	{
		sprintf(path, "%d.png", i + 1);
		game_board[i] = createObject(path, scene, indexToX(i), indexToY(i));
		original_board[i] = game_board[i];
	}

	startbutton = createObject("start.png", scene, 590, 100);
	
	timer = createTimer(timeValue);

	return scene;
}

int main()
{
	srand(time(NULL));

	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);

	setMouseCallback(mouseCallback);
	setTimerCallback(timerCallback);

	startGame(game_init());
}