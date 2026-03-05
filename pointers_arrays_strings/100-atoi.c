#include "main.h"

/**
 * _atoi - converts a string to an integer
 * @s: pointer to string
 *
 * Return: the integer value, or 0 if no numbers
 */
int _atoi(char *s)
{
	int i, sign, result, digit;

	i = 0;
	sign = 1;
	result = 0;
	digit = 0;

	while (s[i] != '\0')
	{
		if (s[i] == '-')
			sign *= -1;
		else if (s[i] == '+')
			sign *= 1;
		else if (s[i] >= '0' && s[i] <= '9')
		{
			digit = s[i] - '0';
			if (sign < 0)
				digit = -digit;
			result = result * 10 + digit;
			if (s[i + 1] < '0' || s[i + 1] > '9')
				break;
		}
		i++;
	}
	return (result);
}
