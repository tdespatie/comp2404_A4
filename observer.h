
// File Name: observer.h
// Created by Tyler Despatie on 11/25/2017.
// Student Number: 101010622
// Assignment: 1
// Course: COMP 2404
// Section: A4



#ifndef ASSIGNMENT_4_OBSERVER_H
#define ASSIGNMENT_4_OBSERVER_H

class Playlist;

class Observer {
public:
    virtual void update(Playlist &subject) = 0;
    virtual void deletion(Playlist &subject) = 0;

    int operator==(const Observer &obs) const {
        return this == &obs;
    }

};


#endif //ASSIGNMENT_4_OBSERVER_H
