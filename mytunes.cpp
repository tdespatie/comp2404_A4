/* * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                   */
/*  Program:  MyTunes Music Player                   */
/*  Author:   Louis Nel                              */
/*  Contributor: Tyler Despatie						 */
/*  Date:     25-Nov-2017                            */
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
using namespace std;
#include "mytunes.h"

MyTunes::MyTunes():view(this)
{
	nextAvailableSongID = 1;
	nextAvailableRecordingID = 1;
	nextAvailableTrackID = 1;
}

void MyTunes::run()
{
	//start user interface
	view.run(); 

}
void MyTunes::executeCommand(Command cmd){

    //execute application (non UI shell) commands
    //These are the commands that affect the data model
    //or retrieve contents from the data model
	if(cmd.isCommand(CMD_ADD)) executeCMDADD(cmd);
    else if(cmd.isCommand(CMD_FOLLOW)) executeCMDFOLLOW(cmd); // New follow command
	else if(cmd.isCommand(CMD_DELETE)) executeCMDDELETE(cmd);
	else if(cmd.isCommand(CMD_SHOW)) executeCMDSHOW(cmd);
}
// Execute follow commands
void MyTunes::executeCMDFOLLOW(Command cmd) {
    enum arguments {FOLLOW, _U, FOLLOWER, _P, PLAYLIST_NAME, _F, TARGET};
    if (StrUtil::toLowerCase(cmd.getToken(TARGET)) == "stop" ) {
        executeDetachPlaylist(cmd);
    } else {
        executeAttachPlaylist(cmd);
    }
}

void MyTunes::executeAttachPlaylist(Command cmd) {
    enum arguments {FOLLOW, _U, FOLLOWER, _P, PLAYLIST_NAME, _F, TARGET};

    User * follower = users.findByID(cmd.getToken(FOLLOWER)); // Find the user specified
    User * target = users.findByID(cmd.getToken(TARGET)); // Find the target specified
    if (follower == nullptr || target == nullptr) { // Check that both the users exist
        view.printOutput("FOLLOW: CANNOT FIND USER SPECIFIED " + cmd.getCommandString());
        return;
    }

    Playlist * follower_playlist = follower->findPlaylist(cmd.getToken(PLAYLIST_NAME)); // Find the follower's playlist
    Playlist * target_playlist = target->findPlaylist(cmd.getToken(PLAYLIST_NAME)); // Find the target's playlist
    if (follower_playlist == nullptr || target_playlist == nullptr) { // Make sure they both exist
        view.printOutput("FOLLOW: CANNOT FIND PLAYLIST SPECIFIED " + cmd.getCommandString());
        return;
    }

    if (target_playlist->checkIfFollowing(*follower)) { // Check to see if the follower is already following the target
        view.printOutput("FOLLOW: " + cmd.getToken(FOLLOWER) + " IS ALREADY FOLLOWING THE PLAYLIST SPECIFIED " + cmd.getCommandString());
        return;
    }

    if (follower_playlist->checkIfFollowing(*target)) { // Check to see if the target is already following the follower
        view.printOutput("FOLLOW" + cmd.getToken(TARGET) + " IS ALREADY FOLLOWING " + cmd.getToken(FOLLOWER) + " " + cmd.getCommandString());
        return;
    }

    if (cmd.getToken(FOLLOWER) == cmd.getToken(TARGET)) { // Check to see if the user is trying to follow himself
        view.printOutput("FOLLOW: USER IS UNABLE TO FOLLOW THEIR OWN PLAYLIST " + cmd.getCommandString() );
        return;
    }
    target_playlist->attach(*follower); // Subscribe the follower to any updates the target has
    follower->update(*target_playlist); // Update the follower's playlist to match the target's
    view.printOutput("EXECUTING: FOLLOW " + cmd.getCommandString());

}

void MyTunes::executeDetachPlaylist(Command cmd) {
    enum arguments {FOLLOW, _U, FOLLOWER, _P, PLAYLIST_NAME, _F, STOP};

    User * follower = users.findByID(cmd.getToken(FOLLOWER)); // Get user specified
    if (follower == nullptr ) { // Check to see if user exists
        view.printOutput("FOLLOW: CANNOT FIND USER SPECIFIED " + cmd.getCommandString());
        return;
    }

    vector<User*> theUsers = users.getCollection(); // Retrieve all the users

    for (auto user : theUsers) { // Find who owns the playlist specified
        if (user->getID() != follower->getID()) { // Don't bother checking the user specified
            Playlist *target_playlist = user->findPlaylist(cmd.getToken(PLAYLIST_NAME)); // Find playlist we wish to stop following
            if (target_playlist != nullptr) { // Make sure it exists
                target_playlist->detach(*follower); // Once we find the playlist we wish to stop following, detach the user
                view.printOutput("EXECUTING: STOPPING FOLLOW " + cmd.getCommandString());
            }
        }
    }
}

//CMD SHOW COMMANDS
void MyTunes::executeCMDSHOW(Command cmd){
    //show recordings
    //show recordings -r recording_id
    //show songs
    //show songs -s song_id
    //show tracks
    //show tracks -t track_id
    //show users
    //show users -u user_id
	
	enum arguments {SHOW, COLLECTION, FLAG, MEMBER_ID};
	
	if(cmd.isTokenAt(COLLECTION, "songs") && !cmd.hasToken("-s")) 
		songs.showOn(view);
	else if(cmd.isTokenAt(COLLECTION, "songs") && cmd.hasToken("-s")) 
		songs.showOn(view, stoi(cmd.getToken("-s")));	
	else if(cmd.isTokenAt(COLLECTION, "recordings") && !cmd.hasToken("-r")) 
		recordings.showOn(view);
	else if(cmd.isTokenAt(COLLECTION, "recordings") && cmd.hasToken("-r")) 
		recordings.showOn(view, stoi(cmd.getToken("-r")));	
	else if(cmd.isTokenAt(COLLECTION, "tracks") && !cmd.hasToken("-t")) 
		tracks.showOn(view);
	else if(cmd.isTokenAt(COLLECTION, "tracks") && cmd.hasToken("-t")) 
		tracks.showOn(view, stoi(cmd.getToken("-t")));	
	else if(cmd.isTokenAt(COLLECTION, "users") && !cmd.hasToken("-u")) 
		users.showOn(view);
	else if(cmd.isTokenAt(COLLECTION, "users") && cmd.hasToken("-u")) 
		users.showOn(view, cmd.getToken("-u"));
    else view.printOutput("EXECUTING: " + cmd.getCommandString());
}

//CMD ADD COMMANDS
void MyTunes::executeCMDADD(Command cmd){
    //add recording
    //add -r id title artist producer year
    //add -r 103 "Beatles For Sale" "Beatles" "George Martin" 1964
    //add -r ? "Beatles For Sale" "Beatles" "George Martin" 1964
    //add song
    //add -s sid title composer //add song
    //add -s 1001 "Misery" "Paul McCartney, John Lennon"
    //add -s ? "Misery" "Paul McCartney, John Lennon"
    //add track
    //add -t trackID albumID songID track_number
    //add -t 12 100 1000 1
	//add -t 12 null 1000 1 //add track not on recording
    //add -t ? 100 1000 1
	//add -t ? null 1000 1  //add track not on recording
    //add user
    //add -u user_id name
    //add -u ajones "Anne Jones"
    //add playlist
    //add -p user_id, playlist_name
    //add -p ajones "Driving Songs"
    //add playlist track
    //add -l user_id playlist_name track_id
    //add -l gjones "mysongs" 6
	
	     if(cmd.hasToken("-s")) executeAddSong(cmd);	
	else if(cmd.hasToken("-r")) executeAddRecording(cmd);	
	else if(cmd.hasToken("-t")) executeAddTrack(cmd);	
	else if(cmd.hasToken("-u")) executeAddUser(cmd);	
	else if(cmd.hasToken("-p")) executeAddPlaylist(cmd);	
	else if(cmd.hasToken("-l")) executeAddPlaylistTrack(cmd);	
	else view.printOutput("EXECUTING: " + cmd.getCommandString());
}

//Individual Add Commands
void MyTunes::executeAddSong(Command cmd){
    //add song
    //add -s sid title composer //add song
    //add -s 1001 "Misery" "Paul McCartney, John Lennon"
    //add -s ? "Misery" "Paul McCartney, John Lennon"
	enum arguments {ADD, _S, ID, TITLE, COMPOSER};
	
	if(!cmd.isValidIndex(cmd.getToken(ID))) return;
	int id = -1;
	if(cmd.isTokenAt(ID,"?")) id = nextAvailableSongID++;
    else id = stoi(cmd.getToken(ID));
	
	if(nextAvailableSongID <= id) nextAvailableSongID = id + 1;
	
	Song* song = new Song(
	  cmd.getToken(TITLE),
	  cmd.getToken(COMPOSER),
	  id
	);
	if(song == nullptr) return;
	songs.add(*song);
	view.printOutput("EXECUTING: ADD SONG " + cmd.getCommandString());
}

void MyTunes::executeAddRecording(Command cmd){
    //add recording
    //add -r id title artist producer year
    //add -r 103 "Beatles For Sale" "Beatles" "George Martin" 1964
    //add -r ? "Beatles For Sale" "Beatles" "George Martin" 1964
	enum arguments {ADD, _R, ID, TITLE, ARTIST, PRODUCER, YEAR};
	
	if(!cmd.isValidIndex(cmd.getToken(ID))) return;
	
	int id = -1;
	if(cmd.isTokenAt(ID,"?")) id = nextAvailableRecordingID++;
    else id = stoi(cmd.getToken(ID));
	if(nextAvailableRecordingID <= id) nextAvailableRecordingID = id + 1;

	Recording* recording = new Recording(
	  cmd.getToken(TITLE),
	  cmd.getToken(ARTIST),
	  cmd.getToken(PRODUCER),
	  cmd.getToken(YEAR),
	  id
	);
	if(recording == nullptr) return;
	recordings.add(*recording);
	view.printOutput("EXECUTING: ADD RECORDING " + cmd.getCommandString());
}
void MyTunes::executeAddTrack(Command cmd){
    //add track
    //add -t trackID albumID songID track_number
    //add -t 12 100 1000 1
	//add -t 12 NULL 1000 1 //add track not on recording
    //add -t ? 100 1000 1
	//add -t ? NULL 1000 1  //add track not on recording
	
	enum arguments {ADD, _T, ID, RECORDING_ID, SONG_ID, TRACK_NUMBER};
	
	if(!cmd.isValidIndex(cmd.getToken(ID))) return;
	if(!cmd.isValidIndex(cmd.getToken(SONG_ID))) return;
	if(!cmd.isValidIndex(cmd.getToken(TRACK_NUMBER))) return;

	int id = -1;
	if(cmd.isTokenAt(ID,"?")) id = nextAvailableTrackID++;
    else id = stoi(cmd.getToken(ID));
	if(nextAvailableTrackID <= id) nextAvailableTrackID = id + 1;
	
	Recording * recording;
	//tracks don't have to be associated with recordings
	if(cmd.getToken(RECORDING_ID).compare("nullptr") == 0) recording = nullptr;
	else {
		if(!cmd.isValidIndex(cmd.getToken(RECORDING_ID))) return;
		recording = recordings.findByID(stoi(cmd.getToken(RECORDING_ID)));
	}

	int track_number = stoi(cmd.getToken(TRACK_NUMBER));
	Track* track = new Track(
	  id, songs.findByID(stoi(cmd.getToken(SONG_ID)))
	);
	if(track == nullptr) return;
	tracks.add(*track);
	if(recording != nullptr) recording->addTrack(*track, track_number);
	view.printOutput("EXECUTING: ADD TRACK " + cmd.getCommandString());
}
void MyTunes::executeAddUser(Command cmd){
    //add user
    //add -u user_id name
    //add -u ajones "Anne Jones"
	enum arguments {ADD, _U, USERID, NAME};
	
	User* user = new User(
	  cmd.getToken(USERID),
	  cmd.getToken(NAME)
	  );
	  
	if(user == nullptr) return;
	users.add(*user);
	view.printOutput("EXECUTING: ADD USER " + cmd.getCommandString());
}
void MyTunes::executeAddPlaylist(Command cmd){
    //add playlist
    //add -p user_id, playlist_name
    //add -p ajones "Driving Songs"
	
	enum arguments {ADD, _P, USERID, PLAYLIST_NAME};
	
	User* user = users.findByID(cmd.getToken(USERID));
	if(user == nullptr) return;
	Playlist * playlist = new Playlist(cmd.getToken(PLAYLIST_NAME));
	if(playlist == nullptr) return;
	user->addPlaylist(*playlist);
	view.printOutput("EXECUTING: ADD PLAYLIST " + cmd.getCommandString());
}
void MyTunes::executeAddPlaylistTrack(Command cmd){
    //add playlist track
    //add -l user_id playlist_name track_id
    //add -l gjones "mysongs" 6
	
	enum arguments {ADD, _L, USERID, PLAYLIST_NAME, TRACK_ID};

	if(!cmd.isValidIndex(cmd.getToken(TRACK_ID))) return;

	User * user = users.findByID(cmd.getToken(USERID));
	if(user == nullptr) return;
	Playlist * playlist = user->findPlaylist(cmd.getToken(PLAYLIST_NAME));
	if(playlist == nullptr) return;
	Track * track = tracks.findByID(stoi(cmd.getToken(TRACK_ID)));
	if(track == nullptr) return;
	playlist->addTrack(*track);
    view.printOutput("EXECUTING: ADD PLAYLIST TRACK " + cmd.getCommandString());
	
}

//CMD DELETE COMMANDS
void MyTunes::executeCMDDELETE(Command cmd){
	if(cmd.hasToken("-u") && cmd.hasToken("-p") && cmd.hasToken("-t") && cmd.hasToken("-GLOBAL")){ 
	    //Cascaded GLOBAL Delete
	    string newCommandString = "delete -t " + cmd.getToken("-t");
	    Command c = Command(newCommandString);
		executeDeleteTrack(c);
	}
	else if(cmd.hasToken("-u") && cmd.hasToken("-p") && cmd.hasToken("-t")) 
		executeDeleteUserPlaylistTrack(cmd);	
	else if(cmd.hasToken("-u") && cmd.hasToken("-p")) 
		executeDeleteUserPlaylist(cmd);	
	else if(cmd.hasToken("-u")) 
		executeDeleteUser(cmd);	
	else if(cmd.hasToken("-r")) 
		executeDeleteRecording(cmd);	
	else if(cmd.hasToken("-t")) 
		executeDeleteTrack(cmd);
    else if(cmd.hasToken("-s")) 
		executeDeleteSong(cmd);		
	else view.printOutput("EXECUTING: " + cmd.getCommandString());
}
void MyTunes::executeDeleteRecording(Command cmd){
	//delete -r 1000 
	enum arguments {DELETE, _R, ID};
	
	if(!cmd.isValidIndex(cmd.getToken(ID))) return;

	Recording* recording = recordings.findByID(stoi(cmd.getToken(ID)));
	if(recording == nullptr) return;
	recordings.remove(*recording);
	view.printOutput("EXECUTING: DELETE RECORDING " + cmd.getCommandString());
}
void MyTunes::executeDeleteUser(Command cmd){
	//delete -u ajones
	enum arguments {DELETE, _U, USER_ID};
	
	User* user = users.findByID(cmd.getToken(USER_ID));
	if(user == nullptr) return;
	users.remove(*user);
	view.printOutput("EXECUTING: DELETE USER " + cmd.getCommandString());
}
void MyTunes::executeDeleteUserPlaylist(Command cmd){
	//delete -u ajones -p favourites
	enum arguments {DELETE, _U, USER_ID, _P, PLAYLIST_NAME};
	string user_id = cmd.getToken("-u");
	string playlistName = cmd.getToken("-p");	
	User* user = users.findByID(user_id);
	if(user == nullptr) return;
	Playlist * playlist = user->findPlaylist(playlistName);
	if(playlist == nullptr) return;
    playlist->notifyDeletion(*playlist);
	user->removePlaylist(*playlist);
	view.printOutput("EXECUTING: DELETE USER PLAYLIST " + cmd.getCommandString());
}
void MyTunes::executeDeleteUserPlaylistTrack(Command cmd){
	//delete -u ajones -p favourites -t 70
	//delete -u ajones -p favourites -t 70 -GLOBAL
	enum arguments {DELETE, _U, USER_ID, _P, PLAYLIST_NAME, _T, TRACK_ID};
	string user_id = cmd.getToken("-u");
	string playlistName = cmd.getToken("-p");	
	string trackIDstring = cmd.getToken("-t");
    if(!cmd.isValidIndex(trackIDstring)) return;
    int trackIndex = stoi(trackIDstring);	
	User* user = users.findByID(user_id);
	if(user == nullptr) return;
	Playlist * playlist = user->findPlaylist(playlistName);
	if(playlist == nullptr) return;
	Track * track = tracks.findByID(trackIndex);
	if(track == nullptr) return;
	playlist->removeTrack(*track);
	view.printOutput("EXECUTING: DELETE USER PLAYLIST TRACK " + cmd.getCommandString());
}
void MyTunes::executeDeleteTrack(Command cmd){
	//delete -t track_id	
	enum arguments {DELETE, _T, TRACK_ID};
	string trackIDstring = cmd.getToken("-t");
    if(!cmd.isValidIndex(trackIDstring)) return;
	int trackIndex = stoi(trackIDstring);
	Track * track = tracks.findByID(trackIndex);
	if(track == nullptr) return;
	
	//PERFORM A CASCADED DELETE
	vector<User*> theUsers = users.getCollection();
	for (auto user : theUsers) {
        user->removeTrack(*track);
	}
	vector<Recording*> theRecordings = recordings.getCollection();
	for (auto recording : theRecordings) {
        recording->removeTrack(*track);
	}
	
	tracks.remove(*track);

}

void MyTunes::executeDeleteSong(Command cmd){
	//delete -s 1000 
	enum arguments {DELETE, _S, ID};
	
	if(!cmd.isValidIndex(cmd.getToken(ID))) return;
	
	Song* song = songs.findByID(stoi(cmd.getToken(ID)));
	if(song == nullptr) return;
	
	//Perform Cascaded Delete
	vector<Track*> theTracks = tracks.getCollection();
	for(auto itr = theTracks.begin(); itr != theTracks.end(); itr++){
		Track* track = *itr;
		Song* trackSong = track->getSong();
		if(song == trackSong){
	       //Cascaded GLOBAL Delete
	       string newCommandString = "delete -t " + to_string(track->getID());
	       Command c = Command(newCommandString);
		   executeDeleteTrack(c);
		}
	}	
	songs.remove(*song);
	view.printOutput("EXECUTING: DELETE SONG " + cmd.getCommandString());
}

