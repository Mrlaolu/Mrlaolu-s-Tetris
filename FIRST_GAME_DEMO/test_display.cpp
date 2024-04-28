#include <iostream>
#include <vector>
#include <windows.h>
#include <conio.h>
#include <random>
#include <thread>
#include <atomic>
#include <mutex>
#include <sstream>

bool debug_mode = 0;

int score = 0;

std::mutex DownMtx;

class display         
{
private:
	int w;
	int h;
	std::vector<std::vector<char>>base;
public:
	display() : w(80), h(60), base(60, std::vector<char>(80, 35)) 
	{
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO cursorInfo;

		// 获取控制台光标信息
		GetConsoleCursorInfo(consoleHandle, &cursorInfo);

		// 设置光标可见性
		cursorInfo.bVisible = false;
		SetConsoleCursorInfo(consoleHandle, &cursorInfo);
	}    //默认显示器大小为80*60

	display(int w, int h) : w(w), h(h), base(h, std::vector<char>(w, '*')) 
	{
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO cursorInfo;

		// 获取控制台光标信息
		GetConsoleCursorInfo(consoleHandle, &cursorInfo);

		// 设置光标可见性
		cursorInfo.bVisible = false;
		SetConsoleCursorInfo(consoleHandle, &cursorInfo);
	}   //自定义显示器大小


	void enough_check()
	{
		DownMtx.lock();
		for (int i = 3; i < h - 1; ++i)
		{
			int cnt = 0;
			for (int j = 1; j < w / 2; ++j)
			{
				if (base[i][j] == '#')cnt++;
			}
			if (cnt >= (w / 2) - 1)
			{
				score += 100;
				for (int p = i; p >= 2; --p)
				{
					for (int j = 1; j < w / 2; ++j)
					{
						base[p][j] = base[p - 1][j];
					}
				}
			}
		}
		DownMtx.unlock();
	}

	void print()
	{
		DownMtx.lock();
		for (int i = 0; i < h + 6; ++i)
		{
			std::cout << "\033[F";
		}
		for (int i = 0; i < h; ++i)
		{
			for (int j = 0; j < w; ++j)
			{
				std::cout << base[i][j];
			}
			std::cout << '\n';
		}
		std::cout << "score: " << score << " " << std::endl;
		std::cout << "A:left    D:right    S:down    W:reverse    X:exit" << std::endl;
		std::cout << std::endl;
		std::cout << "Version:0.10    Made by Mrlaolu" << std::endl;
		DownMtx.unlock();
	}

	void init()
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < w ; ++j)
			{
				base[i][j] = ' ';
			}
		}
		for (int i = 0; i < h - 1; ++i)
		{
			for (int j = 1; j < w / 2; ++j)
			{
				base[i][j] = ' ';
			}
		}
	}

	void fail_exit()
	{
		std::cout << '\r';
		for (int i = 0; i < w; ++i)
		{
			std::cout << " ";
		}
	}
	
	friend class stuff;

};


class stuff                 // 方块
{
protected:
	std::vector<std::vector<int>>construct;
	int x;
	int y;
public:

	stuff(int n):x(1),y(2)                                            //创建当前活动方块
	{

		switch (n)
		{
		case 0:							 // 2*2 square
			construct.resize(2, std::vector<int>(2, 0));
			for (int i = 0; i < 2; ++i)
			{
				for (int j = 0; j < 2; ++j)
				{
					construct[i][j] = 1;
				}
			}
			break;
		case 1:							// 4*1 stick
			construct.resize(4, std::vector<int>(1, 0));
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 1; ++j)
				{
					construct[i][j] = 1;
				}
			}
			break;
		case 2:                               //         ###                  2*3  leftgun
												 //              #
			construct.resize(2, std::vector<int>(3, 0));
			construct[0][0] = construct[0][1] = construct[0][2] = construct[1][2] = 1;
			break;
		case 3:							   //       ###           2*3     rightgun
												//      #
			construct.resize(2, std::vector<int>(3, 0));
			construct[0][0] = construct[0][1] = construct[0][2] = construct[1][0] = 1;
			break;
		case 4:
			//       ##           2*3     leftsnake
		   //          ##
			construct.resize(2, std::vector<int>(3, 0));
			construct[0][0] = construct[0][1] = construct[1][1] = construct[1][2] = 1;
			break;
		case 5:
			//           ##           2*3      rightsnake
		  //          ##
			construct.resize(2, std::vector<int>(3, 0));
			construct[1][0] = construct[1][1] = construct[0][1] = construct[0][2] = 1;
			break;
		case 6:
			//           ###           2*3         tlike
				   //      #
			construct.resize(2, std::vector<int>(3, 0));
			construct[0][0] = construct[0][1] = construct[1][1] = construct[0][2] = 1;
			break;
		}
	}         //

	stuff(int n,display &dis) :x(dis.w / 2 + dis.w / 4), y(7)             //预览下一个方块
	{
		for (int i = -2; i < 5; ++i)
		{
			for (int j = -2; j < 4; ++j)
			{
				dis.base[i + y][j + x] = ' ';
			}
		}
		dis.base[y - 2][x - 1] = 'N'; dis.base[y - 2][x] = 'e'; dis.base[y - 2][x + 1] = 'x'; dis.base[y - 2][x + 2] = 't';
		switch (n)
		{
		case 0:							 // 2*2 square
			construct.resize(2, std::vector<int>(2, 0));
			for (int i = 0; i < 2; ++i)
			{
				for (int j = 0; j < 2; ++j)
				{
					construct[i][j] = 1;
				}
			}
			break;
		case 1:							// 4*1 stick
			construct.resize(4, std::vector<int>(1, 0));
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 1; ++j)
				{
					construct[i][j] = 1;
				}
			}
			break;
		case 2:                               //         ###                  2*3  leftgun
			//              #
			construct.resize(2, std::vector<int>(3, 0));
			construct[0][0] = construct[0][1] = construct[0][2] = construct[1][2] = 1;
			break;
		case 3:							   //       ###           2*3     rightgun
			//      #
			construct.resize(2, std::vector<int>(3, 0));
			construct[0][0] = construct[0][1] = construct[0][2] = construct[1][0] = 1;
			break;
		case 4:
			//       ##           2*3     leftsnake
		   //          ##
			construct.resize(2, std::vector<int>(3, 0));
			construct[0][0] = construct[0][1] = construct[1][1] = construct[1][2] = 1;
			break;
		case 5:
			//           ##           2*3      rightsnake
		  //          ##
			construct.resize(2, std::vector<int>(3, 0));
			construct[1][0] = construct[1][1] = construct[0][1] = construct[0][2] = 1;
			break;
		case 6:
			//           ###           2*3         tlike
				   //      #
			construct.resize(2, std::vector<int>(3, 0));
			construct[0][0] = construct[0][1] = construct[1][1] = construct[0][2] = 1;
			break;
		}
	}          //

	~stuff()
	{}


	void out_display(display& dis)
	{
		DownMtx.lock();
		for (int i = 0; i < construct.size(); ++i)
		{
			for (int j = 0; j < construct[i].size(); ++j)
			{
				if (construct[i][j] != 0)
				{
					dis.base[y + i][x + j] = ' ';
				}
			}
		}
		DownMtx.unlock();
	}

	void in_display(display& dis)
	{
		DownMtx.lock();
		for (int i = 0; i < construct.size(); ++i)
		{
			for (int j = 0; j < construct[i].size(); ++j)
			{
				if (construct[i][j] != 0)
				{
					dis.base[y + i][x + j] = '#';
				}
			}
		}
		DownMtx.unlock();
	}


	bool down(display& dis)
	{
		DownMtx.lock();
		for (int i = 0; i < construct[0].size(); ++i)
		{
			for (int j = construct.size() - 1; j >= 0; --j)
			{
				if (construct[j][i])
				{
					if (dis.base[j + 1 + y][x + i] != ' ')
					{
						DownMtx.unlock();
						return false;
					}
					else
					{
						break;
					}
				}
			}
		}
		y += 1;
		DownMtx.unlock();
		return true;
	}

	void left(display& dis)
	{
		DownMtx.lock();
		for (int j = construct.size() - 1; j >= 0; --j)
		{
			for (int i = 0; i < construct[j].size(); ++i)
			{
				if (construct[j][i] && x > 0)
				{
					if (dis.base[j + y][i - 1 + x] != ' ')
					{
						DownMtx.unlock();
						return;
					}
					else
					{
						break;
					}
				}
			}
		}
		if(x > 1)x -= 1;
		DownMtx.unlock();
	}

	void right(display& dis)
	{
		DownMtx.lock();
		for (int j = construct.size() - 1; j >= 0; --j)
		{
			for (int i = construct[j].size() - 1; i >= 0 ; --i)
			{
				if (construct[j][i])
				{
					if (dis.base[j + y][i + 1 + x] != ' ')
					{
						DownMtx.unlock();
						return;
					}
					else
					{
						break;
					}
				}
			}
		}
		if(x+construct[0].size() < dis.w / 2)x += 1;
		DownMtx.unlock();
	}

	void reverse(display &dis)
	{
		DownMtx.lock();
		std::vector<std::vector<int>>pre_construct(construct[0].size(), std::vector<int>(construct.size()));
		for (int i = 0; i < construct.size(); ++i)
		{
			for (int j = 0; j < construct[i].size(); ++j)
			{
				//pre_construct[j][i] = construct[i][j];
				pre_construct[construct[i].size() - j - 1][i] = construct[i][j];
			}
		} 

		for (int j = pre_construct.size() - 1; j >= 0; --j)          //左右下合法性检测
		{
			for (int i = 0; i < pre_construct[j].size(); ++i)
			{
				if (pre_construct[j][i] && x > 0)
				{
					if (dis.base[j + y][i + x] != ' ')
					{
						DownMtx.unlock();
						return;
					}
					else
					{
						break;
					}
				}
			}
		}

		for (int j = pre_construct.size() - 1; j >= 0; --j)
		{
			for (int i = 0; i < pre_construct[j].size(); ++i)
			{
				if (pre_construct[j][i] && x > 0)
				{
					if (dis.base[j + y][i  + x] != ' ')
					{
						DownMtx.unlock();
						return;
					}
					else
					{
						break;
					}
				}
			}
		}

		for (int i = 0; i < pre_construct[0].size(); ++i)
		{
			for (int j = pre_construct.size() - 1; j >= 0; --j)
			{
				if (pre_construct[j][i])
				{
					if (dis.base[j + y][x + i] != ' ')
					{
						DownMtx.unlock();
						return;
					}
					else
					{
						break;
					}
				}
			}
		}

		construct.assign(pre_construct.begin(), pre_construct.end());
		DownMtx.unlock();
	}

};




int main()
{
	display dis(50,20);
	dis.print();
	dis.init();
	dis.print();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis1(0, 6);

	int n = dis1(gen);
	while (1)
	{
		stuff nowstuff(n);                                             //当前方块
		int k = dis1(gen);
		stuff nextstuff(k,dis);                                      
		nextstuff.in_display(dis);
		nowstuff.in_display(dis);
		dis.print();

		std::atomic_bool StopOuter = false;
		std::atomic_bool StopInner = false;
		
		std::thread AutoTimeDown([&]() -> void {
			while (true)
			{
				nowstuff.out_display(dis);
				if (nowstuff.down(dis) == false)
				{
					StopOuter = true;
					INPUT input;
					input.type = INPUT_KEYBOARD;
					input.ki.wScan = 0;
					input.ki.time = 0;
					input.ki.dwExtraInfo = 0;
					input.ki.wVk = VK_ESCAPE;
					input.ki.dwFlags = 0; // 0 for key press

					SendInput(1, &input, sizeof(INPUT));

					// 模拟键盘抬起事件
					input.ki.dwFlags = KEYEVENTF_KEYUP;
					SendInput(1, &input, sizeof(INPUT));
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				nowstuff.in_display(dis);
				dis.print();
				if(StopOuter)break;
			}
			});

		while (1 && !StopOuter)
		{
			char ch = _getch();
			nowstuff.out_display(dis);
			switch (ch)
			{
			case 'a':
				nowstuff.left(dis);
				break;
			case 'd':
				nowstuff.right(dis);
				break;
			case 's':
				if (nowstuff.down(dis) == false)
				{
					StopInner = true;
				}
				break;
			case 'w':
				nowstuff.reverse(dis);
				break;
			case 'x':
				std::cout << "Do you want to exit?(y/n)";
				while (1)
				{
					bool fail = 0;
					char ch2 = _getch();
					switch (ch2)
					{
						case 'y':
							exit(1);
							break;
						case 'n':
							fail = 1;
							break;
					}
					if (fail) { dis.fail_exit(); break; }
				}
			}



			nowstuff.in_display(dis);
			dis.print();
			if (StopInner)
			{
				StopOuter = true;
				break;
			}
			n = k;
		}
		
		if (StopOuter)
		{
			dis.print();
			dis.enough_check();
			AutoTimeDown.join();
		}

	}
}
	

