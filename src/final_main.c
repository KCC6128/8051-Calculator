#include <8051.h>
#include "delay.h"
#include "LED_Display.h"
#include "Keypad4x4.h"


const char table[14] = { 7,8,9,0,4,5,6,0,1,2,3,0,0,0 };		//LED亮的值轉成整數數值
const char invtable[10] = { 13,8,9,10,4,5,6,0,1,2 };				//整數數值轉成LED亮的值
const char hex_table[16] = { 13,8,9,10,4,5,6,0,1,2,3,20,21,22,17,23 };		//16進制用的表 //0~F

int over[10] = { 0,0,0,0,0,0,0,0,0,0 };				//用來存取歷次的值
char over_count = 0;								//over的計數器
char seg[8] = { 16 ,16, 16, 16, 16, 16, 16, 16 };	//初始化,16在seg_code[]中為全暗
int temp[8] = { 0,0,0,0,0,0,0,0 };		//先把暫存的每個數值歸零
int num[2] = { 0,0 };					//要計算的數值歸零
char error_count = 0;				// 來記錄是否error
char temp_count = 0;				//temp計數器
char num_count = 0;				//num計數器
char oper = 0;					//oper用來存是哪個運算子 //例如加減乘除
int ans = 0;					//運算完的答案
char em = 1;					//用來判斷加減乘除後,輸入數字是否需要清空的計數器
int negative = 1;				//紀錄是否為負號 //正數為1,負數為-1
char initial = 0;				//判斷清空後,第一個數字是否為負數
char carry_temp = 0;				//用來記錄哪個數字要轉進制
char carry_count = 0;				//切換進制
char bin[8] = { 0,0,0,0,0,0,0,0 };	//存放binary
char oct[6] = { 0,0,0,0,0,0 };		//存放Octal
char hex[4] = { 0,0,0,0 };			//存放hexadecimal


//存取歷次的值
void setover(int val) {
	for (unsigned char i = 9; i > 0; i--) {
		over[i] = over[i - 1];	//向右移動
	}
	over[0] = val;		//把新的值存入
}

//設定要亮的值
void setseg(short val) {
	for (unsigned char i = 0; i < 7; i++) {
		seg[i] = seg[i + 1];      //向左移動
	}
	seg[7] = val;			//把新的值放入
}

//把seg[]歸零;
void clean(void) {
	for (unsigned char i = 0; i < 8; i++) {
		seg[i] = 16;							//讓燈全暗
	}

	for (unsigned char j = 0; j < 8; j++) {		//七個位置暫存的值歸零
		temp[j] = 0;
	}

	temp_count = 0;							//算輸入幾個數字到暫存歸零
}

void error(void) {		//亮Error
	setseg(17);
	setseg(18);
	setseg(18);
	setseg(19);
	setseg(18);
}

//把位數轉成十進位的數值
void trans(void) {
	for (unsigned char j = 0; j <= 7; j++) {
		for (unsigned char i = j + 1; i < temp_count; i++) {//輸入的七個數字 看分別是幾位數 再乘10
			temp[j] = temp[j] * 10;
		}
		num[num_count] += temp[j];		//把轉成十幾位的數存進num[]
	}
	num[num_count] = num[num_count] * negative; //看這數字是否為負數
	num_count++;
}

//把int數字轉成個位數矩陣
void invtrans(int a) {
	int loc = 1;
	char count = 0;

	if (a < 0) {	//如果ans小於0,前面補個負號
		a = a * (-1);
		setseg(7);		//亮負值
	}
	int t = a;
	while (t > 0) {		//算ans有幾位數
		count++;
		t /= 10;
	}

	if (a == 0) {
		setseg(13);		//亮0
	}

	else if (a > 0) {
		while (count > 0) {
			if (count > 1) {
				for (unsigned char i = 1; i < count; i++) {		//把ans的位數算出來
					loc *= 10;
				}
			}
			setseg(invtable[a / loc]);		//把數字轉成LED燈亮的值
			a = a - ((a / loc) * loc);
			loc = 1;
			count--;
		}
	}
}

//運算的function
void operation(char oper) {
	if (oper == 3) {	//處理加法
		ans = num[0] + num[1];
	}
	else if (oper == 7) {	//處理減法
		ans = num[0] - num[1];
	}
	else if (oper == 11) {	//處理乘法
		ans = num[0] * num[1];
	}
	else if (oper == 15) {	//處理除法
		if (num[1] != 0) {
			ans = num[0] / num[1];
		}
		else {
			ans = 0;	//處理除零錯誤
			error_count = 1;	//有error
		}
	}
	num[0] = ans;			//為了讓數字能夠做連續運算,所以把答案放回num[0]
	num[1] = 0;				//把num[1]清空
	num_count = 1;			//使得之後的值都放num[1],除非全清除
}

//binary function
void binary(void) {
	for (char k = 0; k < 8; k++) {	//先把bin[]歸零
		bin[k] = 0;
	}
	char i = 0;
	char ne = 0;
	int a;
	a = over[carry_temp];
	if (a < 0) {		//判斷是否為負數,為負數就把ne設為1
		a = a * (-1);
		ne = 1;
	}
	while (a > 0) {				//把數字轉成binary
		bin[i] = a % 2;
		a = a / 2;
		i++;
	}
	if (ne == 1) {			//如果是負數,就執行2的補數
		for (char k = 0; k < 8; k++) {
			bin[k] = 1 - bin[k];
		}
		bin[0] += 1;
		for (char k = 0; k < 7; k++) {
			if (bin[k] == 2) {
				bin[k] = 0;
				bin[k + 1] += 1;
			}
		}
	}
	for (int j = 7; j >= 0; j--) {
		if (bin[j] == 0) {			//亮0
			setseg(13);
		}
		else if (bin[j] == 1) {			//亮1
			setseg(8);
		}
	}
}

//Octal
void Octal(void) {
	for (char k = 0; k < 6; k++) {	//先把oct[]歸零
		oct[k] = 0;
	}
	char i = 0;
	char ne = 0;
	int a;
	a = over[carry_temp];
	if (a < 0) {		//判斷是否為負數,為負數就把ne設為1
		a = a * (-1);
		ne = 1;
	}
	while (a > 0) {				//把數字轉成Octal
		oct[i] = a % 8;
		a = a / 8;
		i++;
	}
	if (ne == 1) {			//如果是負數,就執行8的補數
		for (char k = 0; k < 6; k++) {
			oct[k] = 7 - oct[k];
		}
		oct[0] += 1;
		for (char k = 0; k < 5; k++) {
			if (oct[k] == 8) {
				oct[k] = 0;
				oct[k + 1] += 1;
			}
		}
	}
	for (int j = 5; j >= 0; j--) {		//看值是多少 亮0~7 //借用hex的table
		setseg(hex_table[oct[j]]);
	}
}

//hexadecimal function
void hexadecimal(void) {
	for (char k = 0; k < 4; k++) {	//先把hex[]歸零
		hex[k] = 0;
	}
	char i = 0;
	char ne = 0;
	int a;
	a = over[carry_temp];
	if (a < 0) {		//判斷是否為負數,為負數就把ne設為1
		a = a * (-1);
		ne = 1;
	}
	while (a > 0) {				//把數字轉成hexadecimal
		hex[i] = a % 16;
		a = a / 16;
		i++;
	}
	if (ne == 1) {			//如果是負數,就執行16的補數
		for (char k = 0; k < 4; k++) {
			hex[k] = 15 - hex[k];
		}
		hex[0] += 1;
		for (char k = 0; k < 3; k++) {
			if (hex[k] == 16) {
				hex[k] = 0;
				hex[k + 1] += 1;
			}
		}
	}
	for (int j = 3; j >= 0; j--) {		//看值是多少 亮0~F
		setseg(hex_table[hex[j]]);
	}
}

void main(void) {

	while (1) {
		signed char key = pushBtn();	 //儲存按鈕輸入到變數中

		//如果輸入為數字
		if (key == 0 || key == 1 || key == 2 || key == 4 || key == 5 || key == 6 || key == 8 || key == 9 || key == 10 || key == 13) {
			if (em == 0 && negative == 1) {	//如果之前輸入是運算子則清空 //如果是負號則不用
				clean();
			}
			if (initial == 0 && oper == 7) {	//亮負號
				setseg(7);
			}
			initial = 1;
			em = 1;	
			carry_count = 0;
			temp[temp_count] = table[key];	//把數字存進temp[]
			temp_count++;
			setseg(key);
			delay(500);
		}

		//如果輸入為加減乘除
		if (key == 3 || key == 7 || key == 11 || key == 15) {
			if (em == 1) {		//有輸入數字才計算
				if (oper == 0) {
					trans();	//第一次輸入的數字轉成int
					negative = 1;
					clean();
				}
				else if (oper == 3 || oper == 7 || oper == 11 || oper == 15) {	//如果前一個按鈕為加減乘除,就做運算
					trans();	//第二次輸入的數字轉成int
					negative = 1;
					operation(oper); //運算
					clean();
				}
				else if (oper == 14) {
					clean();
				}
				oper = key; // oper用來存是哪個運算子
			}
			else if (em == 0) {		//上一次輸入為運算子,處理是否為負數
				if (key == 7) {		//如果是負數,就把negative變為負1
					negative = -1;
				}
				clean();
			}
			carry_count = 0;
			em = 0;		//沒放入數字em為0
			setseg(key);
			delay(500);
		}

		//如果輸入為等號
		if (key == 14) {
			em = 1;		
			trans();	//第二次輸入的數字轉成int
			negative = 1;
			operation(oper);	//運算
			clean();
			carry_count = 0;
			if (oper == 3 || oper == 7 || oper == 11 || oper == 15) {		//如果前一個按鈕是加減乘除,就把值存起來
				if (error_count == 0) {				//沒錯誤才存數值
					setover(ans);					//存數值
				}
				over_count = 0;						//確保查歷次資料時,從最近一次計算開始
				carry_temp = over_count;			//因為輸入新的值進去,所以從最新的開始轉進制
			}
			if (oper == 0 || oper == 14) {			//連按等號show歷次值 //如果前一個為清除或是等號,就把歷次的值show出來計算
				ans = over[over_count];				//把歷次的值放入ans
				carry_temp = over_count;			//看是要拿第幾次的資料來轉進制
				over_count++;
				if (over_count > 9) {	//使over_count最多只能是9,因為只記錄10個歷次的值(0~9)
					over_count = 9;
				}
				num[0] = ans;			//為了讓數字能夠做連續運算,所以把答案放回num[0]
				num[1] = 0;				//把num[1]清空
				num_count = 1;			//使得之後的值都放num[1],除非全清除
			}
			if(error_count == 1){		//有錯誤顯示error
				error();
				error_count = 0;
			}
			else {
				invtrans(ans);			//把int數字轉成個位數矩陣
			}
			oper = key;
			delay(500);
		}

		// 清除
		if (key == 12) {
			clean();
			if (carry_count == 0) {	//清空
				error_count = 0;
				initial = 0;
				em = 1;
				negative = 1;
				over_count = 0;
				oper = 0;
				num_count = 0;
				num[0] = 0; num[1] = 0;
				ans = 0;
				carry_count++;
			}
			else if (carry_count == 1) {	//轉2進制
				if (over[carry_temp] < 128 && over[carry_temp] >= (-128)) {	//範圍 -128~127
					binary();
				}
				else {
					error();
				}
				carry_count++;
			}
			else if (carry_count == 2) {	//轉8進制
				if (over[carry_temp] < 32767 && over[carry_temp] >= (-32767)) {	//範圍 -32767~32767
					Octal();
				}
				else {
					error();
				}
				carry_count++;
			}
			else if (carry_count == 3) {	//轉16進制
				if (over[carry_temp] < 32767 && over[carry_temp] >= (-32767)) {	//範圍 -32767~32767
					hexadecimal();
				}
				else {
					error();
				}
				carry_count = 0;
			}
			delay(500);
		}

		for (int i = 0; i < 8; i++) {
			led_display(i, seg[i]);		//display
			delay(5);
		}
	}
}