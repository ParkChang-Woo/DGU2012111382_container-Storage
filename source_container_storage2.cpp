
//��������. �����̳� ������ġ �ڵ�ȭ �˰���
//���ȵ� �˰���









#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include<windows.h>

//�ߵ��� ����,����,���̿� �����̳��� �� ����.
#define yard_x 8
#define yard_y 5
#define yard_z 5
#define order 500

//�����̳� ����
//�ߵ忡 ������ �ð�, �ߵ忡�� ������ �ð�
//�ߵ忡 ������ ����, �ߵ忡�� ������ ����
//����� �ߵ��� ��ġ(x,y,z,�ٸ� �ߵ�)
//���� ����, ���� ����
typedef struct cont {
	int in_time;//= in_day * 24 * 60 + in_hour * 60 + in_minute;

	int order_in;
	int order_out;

	int out_time;//= out_day * 24 * 60 + out_hour * 60 + out_minute;

	int loaded_x, loaded_y, loaded_z, loaded_other_yard;

	bool loaded = true;
	bool unloaded = false;

}cont;



//�ߵ�����
//���� ����, ���ϵ� �����̳��� ��ȣ
typedef struct yd {
	bool charged = false;

	int container_num;
	int top_time;
}yd;

//ũ���� ����

typedef struct crane {
	int cn_x;
	int cn_y;

	int ob_x;
	int ob_y;
	int ob_z;

	int need_sec;
	int state; //0 = ���(C), 1 = �̵���(C)

}cn;


yd yard[yard_x][yard_y][yard_z];
int yd_height[yard_x][yard_y];
int yd_top[yard_x][yard_y];

//�����̳ʸ� ����� �Լ�
void makecontainer(cont cont[]);

//�����̳��� ������ �迭�ϴ� �Լ�
void sort_order(cont conts[], int n, int load_order[], int unload_order[]);


//�����̳��� ������ ����ϴ� �Լ�
void print_info(FILE *fp, cont container[], int load_order[], int unload_order[]);

//�����̳� ���ϸ� �����ϴ� �Լ�
void load(cont conts[], int x, int y, int z, int c, int j);
void load2(cont conts[], int c, int j);

//�����̳ʸ� ���ϸ� �����ϴ� �Լ�
void unload(cont conts[], int x, int y, int z, int c, int j);
void unload2(cont conts[], int c, int j);

//�ܼ� â�� �ߵ��� ��Ȳ�� ����ϴ� �Լ�
void yd_draw(crane);

//���� ����� ����ϴ� �Լ�
void print_result(cont conts[], cn crane);


//���� �Լ�
void main()
{
	FILE  *fp_output;
	int input;

	//������, ����, ���� ����� txt���Ϸ� ���
	fp_output = fopen("output.txt", "w");

	srand((unsigned)time(NULL));
	printf("��������1 : �����̳� ������ġ �ڵ�ȭ �˰��� ���� \n");
	printf("1. ���� �˰��� \n");
	//	printf("2012111382 ��â�� \n\n");

	printf("�ߵ��� ���� ũ��: %d \n�ߵ��� ���� ũ��: %d \n�ߵ��� �ִ� ����: %d \n�����̳� ��: %d \n\n", yard_x, yard_y - 1, yard_z, order);

	printf("�����̳ʸ� �����Ϸ��� �ƹ� Ű�� �Է��ϼ���. \n");

	scanf_s("%c", &input);
	getchar();

	cont container[order];

	//�� �ߵ��� ���̰� 0���� �ʱ�ȭ
	for (int x = 0; x < yard_x; x++)
	{
		for (int y = 0; y < yard_y; y++)
		{
			yd_height[x][y] = 0;
		}
	}
	//ũ���� ����
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
	//�����̳� ���� 
	makecontainer(container);

	sort_order(container, order, load_order, unload_order);

	print_info(fp_output, container, load_order, unload_order);

	printf("������ �۾��� �Ϸ��� �ƹ� Ű�� �Է��ϼ���. \n");

	scanf("%c", &input2);
	getchar();
	int unit = 1; // ���� �ð�

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
	int share = 0; //�ߵ� ������

	bool type = 1;//�˰��� ����

				  //�����̳� ������ ����
				  //8�ð��� �������� �ѷ� ����� �غ���
				  //����!
	while (current_time < 60 * 6 * 14)  //(whole_count != order * 2)
										// ��� �����̳ʰ� ����/���� �� ������ ����
	{
		//�ð��� ������ ���� ȭ�� ���
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




		//���ȭ��
		printf("%u: %d�� %d�� %d�� %d�� \n", current_time, day, hour, minute, second * 10);
		printf("���� ī��Ʈ : %d ���� ī��Ʈ : %d\n", load_count, unload_count);
		printf("�ߵ� ������: %d �ٸ� �ߵ�� �̵��� �����̳� : %d\n", share, other_yard);
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



		//����� ������� ������ ����.

		//ũ������ �۵�


		if (crane.state != 0)
		{
			crane.need_sec -= unit;
		}

		if (crane.need_sec <= 0)
		{
			crane.need_sec = 0;
			crane.state = 0;
		}
		//�����̳� ���� �۾�


		//�ٸ� �ߵ忡 ����� �����̳� ���� ����
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
				//fprintf(fp, "%d(%d:%d:%d)- %d_%d_%d�� %d��° �����̳� ����\n", whole_count, hour, minute, second, x + 1, y + 1, z + 1, c);
				//fprintf(fp_unload, "%d(%d:%d:%d)- %d_%d_%d�� %d��° �����̳� ����\n", unload_count, hour, minute, second, x + 1, y + 1, z + 1, c);

			}
			else
			{
				//unload_count += 1;
				whole_count += 1;
				unload2(container, Out_order, whole_count);
				c_out++;
				Out_order = unload_order[c_out];
				//fprintf(fp, "%d(%d:%d:%d)- �ٸ� �Ƶ��� %d��° �����̳� ����\n", whole_count, hour, minute, second, c);
				//fprintf(fp_unload, "%d(%d:%d:%d)- �ٸ� �Ƶ��� %d��° �����̳� ����\n", unload_count, hour, minute, second, c);
			}
		}






		//���� �����̳� Ž��

		if (container[load_count].in_time < current_time)
		{
			if (crane.state == 0) // �����̳ʰ� ��� �����϶��� �۵�
			{
				if (load_count < order / 2 && container[load_count].order_out >(order - load_count) / 3)
				{
					load_check = 0;
					for (int y = 0; y < yard_y - 1; y++)
					{
						for (int x = 0; x < yard_x; x++)
						{
							if (yd_height[x][y] == 0 && load_check == 0)//���̰� 0��  ���
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
								//fprintf(fp, "%d(%d:%d:%d)- %d ��° �����̳� %d_%d_%d �� ���� ����\n", whole_count, hour, minute, second, load_count, x + 1, y + 1, z + 1);
								//fprintf(fp_load, "%d(%d:%d:%d)- %d ��° �����̳� %d_%d_%d �� ���� ����\n", whole_count, hour, minute, second, load_count, x + 1, y + 1, z + 1);
							}
							else if (yd_height[x][y] > 0 && load_check == 0 && yd_height[x][y] < yard_z)//���̰� 1�̻��ΰ��
							{
								//���� �����̳� ���� ������ ������ ���ϰ�� �ش� ��ġ�� ����
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
									//fprintf(fp, "%d(%d:%d:%d)- %d ��° �����̳� %d_%d_%d �� ���� ����\n", whole_count, hour, minute, second, load_count, x + 1, y + 1, z + 1);
									//fprintf(fp_load, "%d(%d:%d:%d)- %d ��° �����̳� %d_%d_%d �� ���� ����\n", whole_count, hour, minute, second, load_count, x + 1, y + 1, z + 1);
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
							if (yd_height[x][y] == 0 && load_check == 0)//���̰� 0��  ���
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
								//fprintf(fp, "%d(%d:%d:%d)- %d ��° �����̳� %d_%d_%d �� ���� ����\n", whole_count, hour, minute, second, load_count, x + 1, y + 1, z + 1);
								//fprintf(fp_load, "%d(%d:%d:%d)- %d ��° �����̳� %d_%d_%d �� ���� ����\n", whole_count, hour, minute, second, load_count, x + 1, y + 1, z + 1);
							}
							else if (yd_height[x][y] > 0 && load_check == 0 && yd_height[x][y] < yard_z)//���̰� 1�̻��ΰ��
							{
								//���� �����̳� ���� ������ ������ ���ϰ�� �ش� ��ġ�� ����
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
									//fprintf(fp, "%d(%d:%d:%d)- %d ��° �����̳� %d_%d_%d �� ���� ����\n", whole_count, hour, minute, second, load_count, x + 1, y + 1, z + 1);
									//fprintf(fp_load, "%d(%d:%d:%d)- %d ��° �����̳� %d_%d_%d �� ���� ����\n", whole_count, hour, minute, second, load_count, x + 1, y + 1, z + 1);
								}
							}
						}
					}
				}

				//���� ��ġ�� ã�� ������ ��� �ٸ� �ߵ�� �̵�
				if (load_check == 0)
				{
					load2(container, load_count, whole_count);
					other_yard += 1;

					//fprintf(fp, "%d(%d:%d:%d)- %d ��° �����̳� �ٸ� �ߵ忡 �̵�\n", whole_count, hour, minute, second, load_count);
					//fprintf(fp_load, "%d(%d:%d:%d)- %d ��° �����̳� �ٸ� �ߵ忡 �̵�\n", whole_count, hour, minute, second, load_count);
				}
			}
			else// if (crane.state != 0 && container[load_count].in_time == current_time) //ũ������ �۾� ���� ��� �ٸ� �ߵ�� �̵�.
			{
				load2(container, load_count, whole_count);

				other_yard += 1;

				//fprintf(fp, "%d(%d:%d:%d)- %d ��° �����̳� �ٸ� �ߵ忡 �̵�\n", whole_count, hour, minute, second, load_count);
				//fprintf(fp_load, "%d(%d:%d:%d)- %d ��° �����̳� �ٸ� �ߵ忡 �̵�\n", whole_count, hour, minute, second, load_count);
			}
			load_count += 1;
			whole_count += 1;
		}




		//Sleep(100);
		system("cls");
		current_time += unit;
		//�ߵ� ������
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
	//�����̳� �� ����/���� ���� ���
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
//order�� ��ŭ �����̳ʸ� ����� �Լ�
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
//order_in, order_out ��ȣ�� �ű�� �Լ�
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

	printf("%d:: %d ��° �����̳� %d_%d_%d �� ����\n", j, c, x, y, z);

}

void load2(cont conts[], int c, int j)
{
	conts[c].loaded = true;
	conts[c].unloaded = false;
	conts[c].loaded_other_yard = true;
	printf("%d:: %d ��° �����̳� �ٸ� �ߵ�� �̵�\n", j, c);
}

void unload(cont conts[], int x, int y, int z, int c, int j)
{
	yard[x][y][z].charged = false;
	yard[x][y][z].container_num = NULL;
	conts[c].loaded_x = NULL;
	conts[c].loaded_y = NULL;
	conts[c].loaded_z = NULL;
	conts[c].unloaded = true;
	printf("%d:: %d_%d_%d��  %d��° �����̳� ���� \n", j, x, y, z, c);
}

void unload2(cont conts[], int c, int j)
{
	conts[c].unloaded = true;

	printf("%d:: �ٸ� �ߵ��� %d ��° �����̳� ����\n", j, c);
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
		printf("ũ���� �۾���..\n ");
	}
	else
	{
		printf("�����̳� �����\n");
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
	printf("�ߵ� �ִ� ���� �� : %d(%dx%dx%d)\n", yard_z*yard_y*yard_x, yard_x, yard_y - 1, yard_z);
	printf("�����̳� �� :: %d\n", order);
	printf("�ٸ� �ߵ忡 ���ϵ� �����̳� �� :: %d\n", count);




}
