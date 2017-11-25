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

#ifndef _User_H
#define _User_H

#include <ostream>
#include <string>
#include <vector>

#include "playlist.h"

class User {
	/*
	User represents an application user.
	*/
	public:
	User(const string & aUserID, 
	          const string & aName); 
	~User(void);
    int getID();
    //string getUserID();
    string getIDString();
	Playlist * findPlaylist(const string & aPlaylistName);
	void removePlaylist(Playlist & aPlayList);
	void addPlaylist(Playlist & aPlayList);
	void removeTrack(Track & aTrack);
    string toString() const;
	
	private:
	//static int nextUserNumericID;
	string userid;
	string name;
	vector<Playlist*> playlists;
	vector<Playlist*>::iterator findPosition(Playlist & aPlaylist);
	User(const User & aUser); //hide copy constructor in private section
};

ostream & operator<<(ostream & out, const User & aUser);

#endif