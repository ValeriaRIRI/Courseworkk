#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES


#include <stdio.h>
#include <locale.h>
#include <math.h>
#include <time.h>
#define N_TERMS 12

FILE* log_file = NULL; //изначально файл не открыт

double f(double x);
double factorial(int n);
double search_root(double a, double b, double epsilon, double y);
double derivative(double x, double h);
int log_error(const char* message);
int open_log();

//меню реализации программы
int main() {
	setlocale(LC_ALL, "RUS");
	printf("\t\tПрограмма для работы с кусочно-заданной функцией\n");
	int c;

	printf(" ------------------------------------------------------------------\n");
	printf("|       {3root(x)                , x < -2                          |\n");
	printf("| f(x)= { sin(x)/x             , -2 <= x < 0                       |\n");
	printf("|       { sum n=0..12 (-1)^n x^(2n+1)/(2n+1)! , x >= 0             |\n");
	printf("|       { ----------------------                                   |\n");
	printf("|       {        (2n+1)!                                           |\n");
	printf(" ------------------------------------------------------------------\n");

	do {
		printf("\nВыберите операцию: \n");
		printf("1 - Вычисление f(x)\n");
		printf("2 - Таблица значений x -> f(x)\n");
		printf("3 - Поиск корней на интервале\n");
		printf("4 - Вычисление производной f(x) в заданной точке\n");
		printf("0 - Выход из программы\n");
		scanf("%d", &c);
		printf("\nВаш выбор: %d \n", c);
		switch (c) {
			case 1: {
				double x;
				printf("\nВведите x для вычисления функции f(x): ");
				scanf("%lf", &x);
				double res = f(x);

				if (isnan(res)) {
					log_error("f(x) не определена");
				}
				else printf("f(%.5lf)=%.5lf\n", x, res);
				
				break;
			}
			case 2: {
				double x, y, s;
				printf("\nВведите начало интервала: ");
				scanf("%lf", &x);
				printf("Введите конец интервала: ");
				scanf("%lf", &y);
				printf("Введите шаг: ");
				scanf("%lf", &s);
				printf("|\tx  |\tf(x)  |\n");
				printf("|__________|__________|\n");
				if (s == 0 || ((x < y) && s < 0) || ((x > y) && s > 0)) {
					log_error("Шаг выбран неправильно\n");
				}
				else {
					for (double i = x; (s > 0 ? i <= y : i >= y); i += s){
						if (isnan(f(i))) {
							log_error("f(x) не определена.");
							printf("|%10.5lf|	Не определено	|\n", i);
							continue;
						}
						printf("|%10.5lf|%10.5lf|\n", i, f(i));
					}
					printf("|__________|__________|\n");
				}
				break;
			}
			case 3: {
				double a, b;
				double y;
				double epsilon;
				printf("\nВведите начало интервала: ");
				scanf("%lf", &a);
				printf("Введите конец интервала: ");
				scanf("%lf", &b);
				printf("Введите y: ");
				scanf("%lf", &y);
				printf("Введите точность поиска вычислений по y: ");
				scanf("%lf", &epsilon); //требуемая точность вычислений по y (заданное приближение интервала [xn; xk] : xk — xn к нулю)
				double root = search_root(a, b, epsilon, y);
				if (!isnan(root)) {
					printf("\nПри y равном %lf, значение x будет примерно равно %lf, при точности %lf \n", y, root, epsilon);
				}
				else {
					log_error("Корень не найден");
				}
				break;	
			}
			case 4: {
					double x;
					double h;
					printf("\nВведите x для вычисления производной: ");
					scanf("%lf", &x);
					printf("Введите величину приращения(точность) аргумента: ");
					scanf("%lf", &h);
					double df = derivative(x, h);
					if (!isnan(df)) {
						printf("f'(%.6f) = %.10f\n", x, df);
					}
					else {
						log_error("Ошибка вычисления производной.");
					}
				
				break;
			}
			default: {
				printf("Такой операции не существует");
				break;
			}

		}
	}while(c != 0);
	if (log_file) fclose(log_file);
	return 0;
}


//Функция для нахождения факториала
double factorial(int n) { 
	double result = 1;
	if (n < 0) {
		log_error("Ошибка: факториал от отрицательного числа определён неправильно\n");
		
	}
	for (int i = 1; i <= n; i++) {
		result *= i;
	}
	return result;
}

// Вычисление заданной функции
double f(double x) {
	double y;
	if (x < -2) {
		y = cbrt(x); //cbrt - функция для нахождения кубического корня из файла math.h
	}
	else if (x >= -2 && x < 0)  {
		if (fabs(x) < 1e-10) {  // Проверка близости к нулю
			log_error("Значение не определено в точке x=0 (деление на ноль)\n");
			return NAN;
		}
		double denom = pow(x, 2.0);
		if (denom == 0.0) {  //проверка знаменателя
			log_error("Деление на ноль при вычислении sin(x)/x²\n");
			return NAN;
		}
		y = (sin(x)) / pow(x, 2);
	}
	else if (x >= 0) {
		double sum = 0;
		double term;
		for (int n = 0; n <= N_TERMS; n++) {
			term = pow(-1.0, n) * pow(x, 2 * n + 1) / factorial(2 * n + 1);
			sum += term;
		}
		return sum;
	}
}

//Поиск корней функции
double search_root(double a, double b, double epsilon, double y) {
		// решаем f(x) - y = 0 методом деления отрезка пополам
		double fa = f(a) - y;
		double fb = f(b) - y;
		double x;
		
		if (epsilon <= 0) {
			log_error("Точность должна быть положительным числом.\n");
			return NAN;
			
		}
		else if (a == b) {
			log_error("Начало и конец интервала совпадают.\n");
			return NAN;
		}
		// Проверка смены знака на концах интервала (условие существования корня)
		else if (fa * fb > 0) {
			log_error("На заданном интервале нет корня.\n");
			return NAN;
		}
		else {
			double c;
			double fc;
			while (fabs(b - a) > epsilon) {
				c = (a + b) / 2;
				fc = f(c) - y;
				if (fa * fc <= 0) {
					b = c;
					fb = fc;
				}
				else {
					a = c;
					fa = fc;
				}
			}
			x = (a + b) / 2;
			return x;
		}		
}

//Поиск производной. Первый принцип дифференцирования
double derivative(double x, double h) {
	double f1;
	double f2;
	if (h == 0) {
		log_error("h не может быть равен 0");
		return NAN;
	}
	else {
		f1 = f(x + h);
		f2 = f(x - h);
		if (isnan(f1) || isnan(f2)) {
			log_error("Производная не определена в окрестности x");
			return NAN;
		}
		return (f1 - f2) / (2.0 * h);
	}
}

//Функция запуски файла
int open_log() {
	if (!log_file) {  //проверка открыт ли файл
		log_file = fopen("errors.log", "a"); //открытие файла
	}
	return 0;
}

//Функция записи ошибок
int log_error(const char* message) {
	// Проверка, открыт ли лог-файл
	if (!log_file) {
		if (open_log() != 0) {
			printf("КРИТИЧЕСКАЯ ОШИБКА: Не удалось открыть errors.log!\n");
			printf("ОШИБКА: %s\n", message);
			return -1;
		}
	}
	// Текущее время в секундах 
	time_t now = time(NULL);
	// Локальный буфер для форматированной строки времени (64 символа достаточно)
	char time_buf[64];
	// Преобразуем время в структуру tm (локальное время пользователя)
	struct tm* tm_info = localtime(&now);
	// Форматируем время 
	strftime(time_buf, sizeof(time_buf), "[%Y-%m-%d %H:%M:%S]", tm_info);

	// Записываем в лог-файл: время + "ERROR: " + сообщение
	if (fprintf(log_file, "%s ERROR: %s\n", time_buf, message) < 0) {
		printf("ОШИБКА: Не удалось записать в лог-файл!\n");
		printf("ОШИБКА: %s\n", message);
		return -1;
	}

	// Принудительно сбрасываем буфер в файл
	if (fflush(log_file) != 0) {
		printf("ПРЕДУПРЕЖДЕНИЕ: Не удалось сбросить буфер лог-файла!\n");
		return -1;
	}

	// Выводим ошибку в консоль для пользователя
	printf("ОШИБКА: %s\n", message);
	return 0;
}





