#include "main.h"

/**
 * prime_helper - helper to check primality recursively
 * @n: the number to check
 * @i: current divisor to test
 *
 * Return: 1 if no divisor found up to sqrt(n), 0 otherwise
 */
int prime_helper(int n, int i)
{
	if (i * i > n)
		return (1);
	if (n % i == 0)
		return (0);
	return (prime_helper(n, i + 1));
}

/**
 * is_prime_number - returns 1 if n is a prime number, 0 otherwise
 * @n: the number to check
 *
 * Return: 1 if prime, 0 if not prime
 */
int is_prime_number(int n)
{
	if (n <= 1)
		return (0);
	return (prime_helper(n, 2));
}
