#include <stdio.h>

/**
 * add - reads two integers and prints their sum
 */
void add(void)
{
	int a, b;

	printf("A: ");
	scanf("%d", &a);
	printf("B: ");
	scanf("%d", &b);
	printf("Result: %d\n", a + b);
}

/**
 * subtract - reads two integers and prints their difference
 */
void subtract(void)
{
	int a, b;

	printf("A: ");
	scanf("%d", &a);
	printf("B: ");
	scanf("%d", &b);
	printf("Result: %d\n", a - b);
}

/**
 * multiply - reads two integers and prints their product
 */
void multiply(void)
{
	int a, b;

	printf("A: ");
	scanf("%d", &a);
	printf("B: ");
	scanf("%d", &b);
	printf("Result: %d\n", a * b);
}

/**
 * divide - reads two integers and prints the division result
 */
void divide(void)
{
	int a, b;

	printf("A: ");
	scanf("%d", &a);
	printf("B: ");
	scanf("%d", &b);
	if (b == 0)
		printf("Error: division by zero\n");
	else
		printf("Result: %g\n", (float)a / b);
}

/**
 * main - interactive calculator loop
 *
 * Return: 0
 */
int main(void)
{
	int choice;

	while (1)
	{
		printf("Simple Calculator\n");
		printf("1) Add\n");
		printf("2) Subtract\n");
		printf("3) Multiply\n");
		printf("4) Divide\n");
		printf("0) Quit\n");
		printf("Choice: ");
		scanf("%d", &choice);
		if (choice == 0)
		{
			printf("Bye!\n");
			break;
		}
		else if (choice == 1)
			add();
		else if (choice == 2)
			subtract();
		else if (choice == 3)
			multiply();
		else if (choice == 4)
			divide();
		else
			printf("Invalid choice\n");
	}
	return (0);
}
