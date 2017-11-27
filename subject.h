
// File Name: subject.h
// Created by Tyler Despatie on 11/25/2017.
// Student Number: 101010622
// Assignment: 1
// Course: COMP 2404
// Section: A4



#ifndef ASSIGNMENT_4_SUBJECT_H
#define ASSIGNMENT_4_SUBJECT_H


#include "observer.h"
#include "arraylist.h"

class Subject {

    ArrayList <Observer> observers;
public:
    void attach(Observer &obs) {
        observers.add(obs);
        //cout << "\nAttach: " << " observers size: " << observers.size() << "\n";

    }

    void dettach(Observer &obs) {
        observers.remove(obs);
        //cout << "\nDettach: " << " observers size: " << observers.size() << "\n";
    }

    bool checkIfFollowing(Observer &obs) {
        if (observers.isEmpty()) return false;
        ArrayList <Observer> observers_copy = observers; //make copy to iterate over
        for (ArrayList<Observer>::iterator itr = observers_copy.begin(); itr != observers_copy.end(); itr++) {
            if (*itr == obs)
                return true;
        }
    }


    template <typename U>
    void notifyDeletion(U &u) {
        ArrayList <Observer> observers_copy = observers; //make copy to iterate over
        for (ArrayList<Observer>::iterator itr = observers_copy.begin(); itr != observers_copy.end(); itr++) {
            (*itr).deletion(u);
            this->dettach(*itr);
        }
    }
protected:
    template <typename U>
    void notify(U &u) {

        //notify all observers that something about the subject they
        //are watching has changed. concrete subclasses should call
        //this method whenever a state change has ocurred that
        //observers might be interested in

        //Note: observer collection is copied and the copy
        //iterated over so that observers can dettach
        //when they are being notified (i.e. double buffering)

        //In the Gamma pattern this is not done and that code will
        //blow up in a situtation where observers choose to detach as
        //a result of being notified

        ArrayList <Observer> observers_copy = observers; //make copy to iterate over
        for (ArrayList<Observer>::iterator itr = observers_copy.begin(); itr != observers_copy.end(); itr++) {
            (*itr).update(u);
        }
    }

};


#endif //ASSIGNMENT_4_SUBJECT_H
