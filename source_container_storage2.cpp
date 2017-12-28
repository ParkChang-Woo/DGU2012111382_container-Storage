
//개별연구. 컨테이너 보관배치 자동화 알고리즘
//제안된 알고리즘









#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include<windows.h>

//야드의 가로,세로,높이와 컨테이너의 수 설정.
#define yard_x 8
#define yard_y 5
#define yard_z 5
#define order 500

//컨테이너 정보
//야드에 들어오는 시간, 야드에서 나가는 시간
//야드에 들어오는 순서, 야드에서 나가는 순서
//적재된 야드의 위치(x,y,z,다른 야드)
//적하 유무, 양하 유무
typedef struct cont {
	int in_time;//= in_day * 24 * 60 + in_hour * 60 + in_minute;

	int order_in;
	int order_out;

	int out_time;//= out_day * 24 * 60 + out_hour * 60 + out_minute;

	int loaded_x, loaded_y, loaded_z, loaded_other_yard;

	bool loaded = true;
	bool unloaded = false;

}cont;



//야드정보
//적하 유무, 적하된 컨테이너의 번호
typedef struct yd {
	bool charged = false;

	int container_num;
	int top_time;
}yd;

//크레인 정보

typedef struct crane {
	int cn_x;
	int cn_y;

	int ob_x;
	int ob_y;
	int ob_z;

	int need_sec;
	int state; //0 = 대기(C), 1 = 이동중(C)

}cn;


yd yard[yard_x][yard_y][yard_z];
int yd_height[yard_x][yard_y];
int yd_top[yard_x][yard_y];

//컨테이너를 만드는 함수
void makecontainer(cont cont[]);

//컨테이너의 순서를 배열하는 함수
void sort_order(cont conts[], int n, int load_order[], int unload_order[]);


//컨테이너의 정보를 출력하는 함수
void print_info(FILE *fp, cont container[], int load_order[], int unload_order[]);

//컨테이너 적하를 실행하는 함수
void load(cont conts[], int x, int y, int z, int c, int j);
void load2(cont conts[], int c, int j);

//컨테이너를 양하를 실행하는 함수
void unload(cont conts[], int x, int y, int z, int c, int j);
void unload2(cont conts[], int c, int j);

//콘솔 창에 야드의 상황을 출력하는 함수
void yd_draw(crane);

//최종 결과를 출력하는 함수
void print_result(cont conts[], cn crane);


//메인 함수
void main()
{
	FILE  *fp_output;
	int input;

	//양적하, 적하, 양하 기록을 txt파일로 출력
	fp_output = fopen("output.txt", "w");

	srand((unsigned)time(NULL));
	printf("개별연구1 : 컨테이너 보관배치 자동화 알고리즘 연구 \n");
	printf("1. 기존 알고리즘 \n");
	//	printf("2012111382 박창우 \n\n");

	printf("야드의 가로 크기: %d \n야드의 세로 크기: %d \n야드의 최대 높이: %d \n컨테이너 수: %d \n\n", yard_x, yard_y - 1, yard_z, order);

	printf("컨테이너를 설정하려면 아무 키나 입력하세요. \n");

	scanf_s("%c", &input);
	getchar();

	cont container[order];

	//각 야드의 높이값 0으로 초기화
	for (int x = 0; x < yard_x; x++)
	{
		for (int y = 0; y < yard_y; y++)
		{
			yd_height[x][y] = 0;
		}
	}
	//크레인 설정
	cn crane;
	crane.cn_x = 0;
	crane.cn_y = yard_y - 1;
	crane.state = 0;
	crane.need_sec = 0;

	int load_order[order];
	int unload_order[order];

	int input2;
	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////
	//컨테이너 설정 
	makecontainer(container);

	sort_order(container, order, load_order, unload_order);

	print_info(fp_output, container, load_order, unload_order);

	printf("양적하 작업을 하려면 아무 키나 입력하세요. \n");

	scanf("%c", &input2);
	getchar();
	int unit = 1; // 단위 시간

	int load_count = 0;
	int unload_count = 0;
	int whole_count = 0;

	unsigned int current_time = 0;
	int other_yard = 0;

	int load_check;
	int c_in = 1;
	int c_out = 1;
	int Out_order = 0;
	int work = 0;

	Out_order = unload_order[c_out];
	int share = 0; //야드 점유율

	bool type = 1;//알고리즘 종류

				  //컨테이너 양적하 시작
				  //8시간을 기준으로 총량 계산을 해보자
				  //시작!
	while (current_time < 60 * 6 * 14)  //(whole_count != order * 2)
										// 모든 컨테이너가 적하/양하 될 때까지 진행
	{
		//시간을 포함한 현재 화면 출력
		int day = 0;
		int hour = 0;
		int minute = current_time / 6;

		while (minute >= 60)
		{
			minute -= 60;
			hour += 1;
		}
		while (hour >= 24)
		{
			hour -= 24;
			day += 1;
		}
		int second = (int)(current_time % 6);




		//출력화면
		printf("%u: %d일 %d시 %d분 %d초 \n", current_time, day, hour, minute, second * 10);
		printf("적재 카운트 : %d 반출 카운트 : %d\n", load_count, unload_count);
		printf("야드 점유율: %d 다른 야드로 이동한 컨테이너 : %d\n", share, other_yard);
		//printf("work:%d  nedd_sec:%d\n", work, crane.need_sec);
		yd_draw(crane);

		//for (int y = 0; y < yard_y; y++) {
		//	for (int x = 0; x < yard_x; x++) {

		//		//int num_c = yard[x][y][yd_height[x][y]-1].container_num;
		//			//	printf("%d ", container[num_c].out_time);
		//		printf("%d ", yd_top[x][y]);
		//		}
		//	printf("\n");
		//}

		if (current_time % 100 == 0)
		{
			fprintf(fp_output, "%d\n", share);
		}



		//적재는 순서대로 반출은 랜덤.

		//크레인의 작동


		if (crane.state != 0)
		{
			crane.need_sec -= unit;
		}

		if (crane.need_sec <= 0)
		{
			crane.need_sec = 0;
			crane.state = 0;
		}
		//컨테이너 반출 작업


		//다른 야드에 적재된 컨테이너 반출 실행
		if (container[Out_order].out_time < current_time)
		{
			if (container[Out_order].loaded_other_yard == false)
			{
				int x = container[Out_order].loaded_x;
				int y = container[Out_order].loaded_y;
				int z = container[Out_order].loaded_z;

				unload_count += 1;
				whole_count += 1;
				yd_height[x][y] -= 1;
				int tmp = yard[x][y][z - 1].container_num;
				yd_top[x][y] = container[tmp].out_time;
				unload(container, x, y, z, Out_order, whole_count);

				crane.state = 1;
				crane.ob_x = x - crane.cn_x;
				crane.ob_y = y - crane.cn_y;
				crane.ob_z = z;
				if (crane.ob_x < 0) crane.ob_x*-1;
				if (crane.ob_y > 0) crane.ob_y*-1;
				crane.need_sec = 12 + crane.ob_x * 3 + crane.ob_y * 6 - (crane.ob_z * 1);

				c_out++;
				Out_order = unload_order[c_out];
				//fprintf(fp, "%d(%d:%d:%d)- %d_%d_%d의 %d번째 컨테이너 반출\n", whole_count, hour, minute, second, x + 1, y + 1, z + 1, c);
				//fprintf(fp_unload, "%d(%d:%d:%d)- %d_%d_%d의 %d번째 컨테이너 반출\n", unload_count, hour, minute, second, x + 1, y + 1, z + 1, c);

			}
			else
			{
				//unload_count += 1;
				whole_count += 1;
				unload2(container, Out_order, whole_count);
				c_out++;
				Out_order = unload_order[c_out];
				//fprintf(fp, "%d(%d:%d:%d)- 다른 아드의 %d번째 컨테이너 반출\n", whole_count, hour, minute, second, c);
				//fprintf(fp_unload, "%d(%d:%d:%d)- 다른 아드의 %d번째 컨테이너 반출\n", unload_count, hour, minute, second, c);
			}
		}






		//적재 컨테이너 탐색

		if (container[load_count].in_time < current_time)
		{
			if (crane.state == 0) // 컨테이너가 대기 상태일때만 작동
			{
				if (load_count < order / 2 && container[load_count].order_out >(order - load_count) / 3)
				{
					load_check = 0;
					for (int y = 0; y < yard_y - 1; y++)
					{
						for (int x = 0; x < yard_x; x++)
						{
							if (yd_height[x][y] == 0 && load_check == 0)//높이가 0인  경우
							{

								int z = 0;
								yd_height[x][y] = 1;
								yd_top[x][y] = container[load_count].out_time;
								load(container, x, y, z, load_count, whole_count);

								crane.state = 1;
								crane.ob_x = x - crane.cn_x;
								crane.ob_y = y - crane.cn_y;
								crane.ob_z = z;
								if (crane.ob_x < 0) crane.ob_x*-1;
								if (crane.ob_y > 0) crane.ob_y*-1;
								crane.need_sec = 12 + crane.ob_x * 3 + crane.ob_y * 6 - (crane.ob_z * 1);
								load_check = 1;
								//fprintf(fp, "%d(%d:%d:%d)- %d 번째 컨테이너 %d_%d_%d 에 적재 시작\n", whole_count, hour, minute, second, load_count, x + 1, y + 1, z + 1);
								//fprintf(fp_load, "%d(%d:%d:%d)- %d 번째 컨테이너 %d_%d_%d 에 적재 시작\n", whole_count, hour, minute, second, load_count, x + 1, y + 1, z + 1);
							}
							else if (yd_height[x][y] > 0 && load_check == 0 && yd_height[x][y] < yard_z)//높이가 1이상인경우
							{
								//밑의 컨테이너 보다 나가는 순서가 앞일경우 해당 위치에 적재
								if (container[load_count].out_time < yd_top[x][y])
								{
									int tmp = container[load_count].out_time;
									//int compare_z = yd_height[x][y] - 1;
									//int compare = yard[x][y][compare_z].container_num;

									int z = yd_height[x][y];
									yd_height[x][y] += 1;
									yd_top[x][y] = tmp;
									load(container, x, y, z, load_count, whole_count);

									crane.state = 1;
									crane.ob_x = x - crane.cn_x;
									crane.ob_y = y - crane.cn_y;
									crane.ob_z = z;
									if (crane.ob_x < 0) crane.ob_x*-1;
									if (crane.ob_y > 0) crane.ob_y*-1;
									crane.need_sec = 12 + crane.ob_x * 3 + crane.ob_y * 6 - (crane.ob_z * 1);
									load_check = 1;
									//fprintf(fp, "%d(%d:%d:%d)- %d 번째 컨테이너 %d_%d_%d 에 적재 시작\n", whole_count, hour, minute, second, load_count, x + 1, y + 1, z + 1);
									//fprintf(fp_load, "%d(%d:%d:%d)- %d 번째 컨테이너 %d_%d_%d 에 적재 시작\n", whole_count, hour, minute, second, load_count, x + 1, y + 1, z + 1);
								}
							}
						}
					}
				}
				else
				{
					load_check = 0;
					for (int y = yard_y - 2; y >= 0; y--)
					{
						for (int x = 0; x < yard_x; x++)
						{
							if (yd_height[x][y] == 0 && load_check == 0)//높이가 0인  경우
							{

								int z = 0;
								yd_height[x][y] = 1;
								yd_top[x][y] = container[load_count].out_time;
								load(container, x, y, z, load_count, whole_count);

								crane.state = 1;
								crane.ob_x = x - crane.cn_x;
								crane.ob_y = y - crane.cn_y;
								crane.ob_z = z;
								if (crane.ob_x < 0) crane.ob_x*-1;
								if (crane.ob_y > 0) crane.ob_y*-1;
								crane.need_sec = 12 + crane.ob_x * 3 + crane.ob_y * 6 - (crane.ob_z * 1);
								load_check = 1;
								//fprintf(fp, "%d(%d:%d:%d)- %d 번째 컨테이너 %d_%d_%d 에 적재 시작\n", whole_count, hour, minute, second, load_count, x + 1, y + 1, z + 1);
								//fprintf(fp_load, "%d(%d:%d:%d)- %d 번째 컨테이너 %d_%d_%d 에 적재 시작\n", whole_count, hour, minute, second, load_count, x + 1, y + 1, z + 1);
							}
							else if (yd_height[x][y] > 0 && load_check == 0 && yd_height[x][y] < yard_z)//높이가 1이상인경우
							{
								//밑의 컨테이너 보다 나가는 순서가 앞일경우 해당 위치에 적재
								if (container[load_count].out_time < yd_top[x][y])
								{
									int tmp = container[load_count].out_time;
									//int compare_z = yd_height[x][y] - 1;
									//int compare = yard[x][y][compare_z].container_num;

									int z = yd_height[x][y];
									yd_height[x][y] += 1;
									yd_top[x][y] = tmp;
									load(container, x, y, z, load_count, whole_count);

									crane.state = 1;
									crane.ob_x = x - crane.cn_x;
									crane.ob_y = y - crane.cn_y;
									crane.ob_z = z;
									if (crane.ob_x < 0) crane.ob_x*-1;
									if (crane.ob_y > 0) crane.ob_y*-1;
									crane.need_sec = 12 + crane.ob_x * 3 + crane.ob_y * 6 - (crane.ob_z * 1);
									load_check = 1;
									//fprintf(fp, "%d(%d:%d:%d)- %d 번째 컨테이너 %d_%d_%d 에 적재 시작\n", whole_count, hour, minute, second, load_count, x + 1, y + 1, z + 1);
									//fprintf(fp_load, "%d(%d:%d:%d)- %d 번째 컨테이너 %d_%d_%d 에 적재 시작\n", whole_count, hour, minute, second, load_count, x + 1, y + 1, z + 1);
								}
							}
						}
					}
				}

				//적재 위치를 찾지 못했을 경우 다른 야드로 이동
				if (load_check == 0)
				{
					load2(container, load_count, whole_count);
					other_yard += 1;

					//fprintf(fp, "%d(%d:%d:%d)- %d 번째 컨테이너 다른 야드에 이동\n", whole_count, hour, minute, second, load_count);
					//fprintf(fp_load, "%d(%d:%d:%d)- %d 번째 컨테이너 다른 야드에 이동\n", whole_count, hour, minute, second, load_count);
				}
			}
			else// if (crane.state != 0 && container[load_count].in_time == current_time) //크레인이 작업 중일 경우 다른 야드로 이동.
			{
				load2(container, load_count, whole_count);

				other_yard += 1;

				//fprintf(fp, "%d(%d:%d:%d)- %d 번째 컨테이너 다른 야드에 이동\n", whole_count, hour, minute, second, load_count);
				//fprintf(fp_load, "%d(%d:%d:%d)- %d 번째 컨테이너 다른 야드에 이동\n", whole_count, hour, minute, second, load_count);
			}
			load_count += 1;
			whole_count += 1;
		}




		//Sleep(100);
		system("cls");
		current_time += unit;
		//야드 점유율
		share = 0;
		for (int x = 0; x < yard_x; x++)
		{
			for (int y = 0; y < yard_y - 1; y++)
			{
				share += yd_height[x][y];
			}
		}

	}

	print_result(container, crane);


	fclose(fp_output);


}




void print_info(FILE *fp, cont container[], int load_order[], int unload_order[])
{
	//컨테이너 및 적하/양하 순서 출력
	for (int j = 0; j < order; j++)
	{
		printf("container %d  load: %d  unload: %d \n",
			j, container[j].in_time, container[j].out_time);

		fprintf(fp, "container %d  load: %d  unload: %d \n",
			j, container[j].in_time, container[j].out_time);
	}
	printf("\n");


	/*printf("unload order :: ");
	for (int i = 0; i < order; i++)
	{
	printf("%d ", unload_order[i]);
	}
	printf("\n");*/
}
void makecontainer(cont conts[])
//order량 만큼 컨테이너를 만드는 함수
{
	int vol = 80;
	int i = 1;
	int i1 = 0;

	while (i < order)
	{
		conts[i].in_time = i * 180 + i1 * 6 * 10;
		conts[i].in_time = conts[i].in_time / 10;

		int i0 = i % vol;
		if (i0 == 0)
		{
			i1 += 1;
		}
		int ran_hour = rand() % 8;//24;
		int ran_min = rand() % 60;
		int ran_sec = rand() % 6;

		conts[i].out_time = conts[i].in_time + ran_hour * 60 * 6 + ran_min * 6 + ran_sec;
		i++;
	}


	//for (int i0 = 0; i0 < order / vol ; i0++)
	//{
	//	int random_hour = (i0)* 1;

	//	for (int i1 = 0; i1 < vol; i1++)
	//	{
	//		conts[i1 + i0 * vol].in_time = random_hour * 60 * 60 + i1 * 90;

	//		int ran_hour = rand() % 24;//24;
	//		int ran_min = rand() % 60;
	//		int ran_sec = rand() % 60;

	//		conts[i1 + i0 * vol].out_time = conts[i1 + i0 * vol].in_time + ran_hour * 60 * 60 + ran_min * 60 + ran_sec;

	//		conts[i1 + i0 * vol].order_in = i1 + i0 * vol;
	//		conts[i1 + i0 * vol].order_out = i1 + i0 * vol;
	//	}
	//}
	conts[0].in_time = 0;
	conts[0].out_time = 0;
}

void sort_order(cont conts[], int n, int load_order[], int unload_order[])
//order_in, order_out 번호를 매기는 함수
{
	int time = 0;
	int order_in_num = 0;
	int order_out_num = 0;

	while (order_out_num < order)
	{
		for (int i = 0; i < order; i++)
		{
			if (conts[i].out_time == time)
			{
				conts[i].order_out = order_out_num;
				unload_order[order_out_num] = i;
				order_out_num += 1;
			}
		}
		time += 1;
	}
}

void load(cont conts[], int x, int y, int z, int c, int j)
{
	yard[x][y][z].charged = true;
	yard[x][y][z].container_num = c;


	conts[c].loaded_x = x;
	conts[c].loaded_y = y;
	conts[c].loaded_z = z;
	conts[c].loaded_other_yard = false;
	conts[c].loaded = true;
	conts[c].unloaded = false;

	printf("%d:: %d 번째 컨테이너 %d_%d_%d 에 적재\n", j, c, x, y, z);

}

void load2(cont conts[], int c, int j)
{
	conts[c].loaded = true;
	conts[c].unloaded = false;
	conts[c].loaded_other_yard = true;
	printf("%d:: %d 번째 컨테이너 다른 야드로 이동\n", j, c);
}

void unload(cont conts[], int x, int y, int z, int c, int j)
{
	yard[x][y][z].charged = false;
	yard[x][y][z].container_num = NULL;
	conts[c].loaded_x = NULL;
	conts[c].loaded_y = NULL;
	conts[c].loaded_z = NULL;
	conts[c].unloaded = true;
	printf("%d:: %d_%d_%d의  %d번째 컨테이너 반출 \n", j, x, y, z, c);
}

void unload2(cont conts[], int c, int j)
{
	conts[c].unloaded = true;

	printf("%d:: 다른 야드의 %d 번째 컨테이너 반출\n", j, c);
}



void yd_draw(cn crane)
{
	for (int y = 0; y < yard_y - 1; y++) {
		for (int x = 0; x < yard_x; x++) {

			printf("%d ", yd_height[x][y]);

		}
		printf("\n");
	}
	if (crane.state != 0)
	{
		printf("크레인 작업중..\n ");
	}
	else
	{
		printf("컨테이너 반출로\n");
	}


}


void print_result(cont conts[], cn crane)
{
	yd_draw(crane);
	printf("\n");

	printf("\n");
	int count = 0;
	for (int c = 0; c<order; c++)
	{
		if (conts[c].loaded_other_yard == true)
		{
			count += 1;
			//printf("%d ", c);
		}
	}
	printf("야드 최대 적재 수 : %d(%dx%dx%d)\n", yard_z*yard_y*yard_x, yard_x, yard_y - 1, yard_z);
	printf("컨테이너 수 :: %d\n", order);
	printf("다른 야드에 적하된 컨테이너 수 :: %d\n", count);




}
