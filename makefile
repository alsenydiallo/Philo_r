FLAGS = -std=c99 -Wall -pedantic
COMP = gcc
DEBUG = -g
LIB = -lm -lpthread

philo: philo_r.c random_r.c random_r.h
	$(COMP) $(FLAGS) philo_r.c random_r.c $(LIB) -o philo_r

clean:
	rm philo_r

