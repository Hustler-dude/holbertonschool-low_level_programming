#include <stdlib.h>
#include "dog.h"

/**
 * _len - returns the length of a string
 * @s: the string
 *
 * Return: length of s
 */
static unsigned int _len(char *s)
{
	unsigned int i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

/**
 * _copy - copies a string into dest
 * @dest: destination buffer
 * @src: source string
 */
static void _copy(char *dest, char *src)
{
	unsigned int i;

	i = 0;
	while (src[i])
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}

/**
 * new_dog - creates a new dog
 * @name: the dog's name
 * @age: the dog's age
 * @owner: the dog's owner
 *
 * Return: pointer to new dog_t, or NULL on failure
 */
dog_t *new_dog(char *name, float age, char *owner)
{
	dog_t *dog;

	dog = malloc(sizeof(dog_t));
	if (dog == NULL)
		return (NULL);

	dog->name = malloc(_len(name) + 1);
	if (dog->name == NULL)
	{
		free(dog);
		return (NULL);
	}
	_copy(dog->name, name);

	dog->owner = malloc(_len(owner) + 1);
	if (dog->owner == NULL)
	{
		free(dog->name);
		free(dog);
		return (NULL);
	}
	_copy(dog->owner, owner);

	dog->age = age;
	return (dog);
}
