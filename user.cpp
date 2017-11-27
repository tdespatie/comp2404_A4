/* * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                   */
/*  Program:  MyTunes Music Player                   */
/*  Author:   Louis Nel                              */
/*  Date:     21-SEP-2017                            */
/*                                                   */
/*  (c) 2017 Louis Nel                               */
/*  All rights reserved.  Distribution and           */
/*  reposting, in part or in whole, requires         */
/*  written consent of the author.                   */
/*                                                   */
/*  COMP 2404 students may reuse this content for    */
/*  their course assignments without seeking consent */
/* * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>
#include <string>

using namespace std;

#include "user.h"

//int User::nextUserNumericID = 1000;

User::User(const string &aUserID, const string &aName) {
    cout << "User(string&, string&, int)" << endl;
    userid = aUserID;
    name = aName;
    //id = nextUserNumericID++;
}

User::User(const User &aUser) {
    cout << "User(const User & aUser)" << endl;
    cout << "ERROR: User(const User & aUser) --should never run" << endl;
}

User::~User() {
    cout << "~User(void)" << endl;
    //user objects own playlists
    for (auto &playlist : playlists)
        delete playlist; //delete playlists this user owns

}

int User::getID() {
    //return a simple hash of the userid
    //This is not used in this application be just serves as a reasonable numeric
    //id to return
    int hash = 0;
    for (unsigned i = 0; i < userid.length(); ++i) {
        hash += (i + 1) * userid.at(i);
    }
    return hash;
}

string User::getIDString() { return userid; }

vector<Playlist *>::iterator User::findPosition(Playlist &aPlaylist) {
    for (auto itr = playlists.begin(); itr != playlists.end(); ++itr)
        if (*itr == &aPlaylist) return itr;
    return playlists.end();
}

Playlist *User::findPlaylist(const string &aPlaylistName) {
    for (auto &playlist : playlists)
        if (aPlaylistName == playlist->getName()) return playlist;
    return nullptr;
}

void User::addPlaylist(Playlist &aPlaylist) {
    //add playlist if it does not already exist
    auto itr = findPosition(aPlaylist);
    if (itr == playlists.end()) {
        playlists.push_back(&aPlaylist);
    }
}

void User::removePlaylist(Playlist &aPlaylist) {
    auto itr = findPosition(aPlaylist);
    if (itr != playlists.end()) {
        Playlist *playlist = *itr;
        playlists.erase(itr);
        delete playlist;
    }
}

void User::removeTrack(Track &aTrack) {
    for (auto playlist : playlists)
        playlist->removeTrack(aTrack);
}

string User::toString() const {
    string indent = "     ";
    string s;
    s.append(userid + " " + name);
    s.append("\n");
    s.append(indent + "Playlists:\n");
    for (vector<Playlist *>::size_type i = 0; i < playlists.size(); i++) {
        s.append(indent + to_string(i) + " " + (playlists[i])->toString() + "\n");
        s.append("\n");
    }

    return s;
}

ostream &operator<<(ostream &out, const User &aUser) {
    out << aUser.toString() << endl;
    return out;
}

// This is our function used to handle updates to the Playlists we follow
void User::update(Playlist &target)  {
    Playlist *followerPlaylist = this->findPlaylist(target.getName()); // Find the playlist which needs to be changed
    vector<Track*>& tracks = followerPlaylist->getTracks();
    for (auto itr = tracks.rbegin(); itr != tracks.rend(); ++itr )  // Remove all the tracks from the playlist
        followerPlaylist->removeTrack(**itr);


    for (auto track : target.getTracks())  // Add all the tracks the playlist we are following has
        followerPlaylist->addTrack(*track);

};

// This is our function used to handle what happens when a playlist we are following gets deleted
void User::deletion(Playlist &target) {
    Playlist *followerPlaylist = this->findPlaylist(target.getName());
    target.detach(*this); // We need to detach all the observers that are following this playlist
    cout << "TARGET PLAYLIST: " << followerPlaylist->getName() << " HAS BEEN DELETED." << endl;
}
