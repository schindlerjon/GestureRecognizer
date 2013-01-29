#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "wiimote.h"
#include "gestAlgo.h"
#include <mmsystem.h>
#include <WINDOWS.H>
#include <math.h>
#include <map>
#include <vector>

#define PI 3.14159265
#define WINDOW 100
#define NUMPOINTS 64

using namespace std;



//Current Version: 

//Authors: Trey Roady & Jonathan Schindler
//Texas A&M University 
//Human Factors Lab

/*************************************************************************************
*************************************************************************************/

static const TCHAR* wait_str[] = { _T(".  "), _T(".. "), _T("...") };
static const float A_THRESH = 1.6f;

Gesture makeNewGesture(wiimote &remote);
vector<Vector2D> recordSignal(wiimote &remote);
Gesture addSampleData(Gesture T, wiimote &remote);

void writeGestures(vector<Gesture> gestureList);
vector<Gesture> importGestures();

RecordScore scoreGesture(vector<Gesture> templates, wiimote &remote);

void editGestures(vector<Gesture> &gestureList, wiimote &remote);
void printGestures(vector<Gesture> gestureList);

void on_state_change (wiimote			  &remote,
					  state_change_flags  changed,
					  const wiimote_state &new_state)
	{
	// we use this callback to set report types etc. to respond to key events
	//  (like the wiimote connecting or extensions (dis)connecting).
	
	// NOTE: don't access the public state from the 'remote' object here, as it will
	//		  be out-of-date (it's only updated via RefreshState() calls, and these
	//		  are reserved for the main application so it can be sure the values
	//		  stay consistent between calls).  Instead query 'new_state' only.

	// the wiimote just connected
	if(changed & CONNECTED)
		{
		// ask the wiimote to report everything (using the 'non-continous updates'
		//  default mode - updates will be frequent anyway due to the acceleration/IR
		//  values changing):

		// note1: you don't need to set a report type for Balance Boards - the
		//		   library does it automatically.
		
		// note2: for wiimotes, the report mode that includes the extension data
		//		   unfortunately only reports the 'BASIC' IR info (ie. no dot sizes),
		//		   so let's choose the best mode based on the extension status:
		if(new_state.ExtensionType != wiimote::BALANCE_BOARD)
			{
			if(new_state.bExtension)
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT); // no IR dots
			else
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);		//    IR dots
			}
		}
	// a MotionPlus was detected
	if(changed & MOTIONPLUS_DETECTED)
		{
		// enable it if there isn't a normal extension plugged into it
		// (MotionPlus devices don't report like normal extensions until
		//  enabled - and then, other extensions attached to it will no longer be
		//  reported (so disable the M+ when you want to access them again).
		if(remote.ExtensionType == wiimote_state::NONE) {
			bool res = remote.EnableMotionPlus();
			_ASSERT(res);
			}
		}
	// an extension is connected to the MotionPlus
	else if(changed & MOTIONPLUS_EXTENSION_CONNECTED)
		{
		// We can't read it if the MotionPlus is currently enabled, so disable it:
		if(remote.MotionPlusEnabled())
			remote.DisableMotionPlus();
		}
	// an extension disconnected from the MotionPlus
	else if(changed & MOTIONPLUS_EXTENSION_DISCONNECTED)
		{
		// enable the MotionPlus data again:
		if(remote.MotionPlusConnected())
			remote.EnableMotionPlus();
		}
	// another extension was just connected:
	else if(changed & EXTENSION_CONNECTED)
		{
#ifdef USE_BEEPS_AND_DELAYS
		Beep(1000, 200);
#endif
		// switch to a report mode that includes the extension data (we will
		//  loose the IR dot sizes)
		// note: there is no need to set report types for a Balance Board.
		if(!remote.IsBalanceBoard())
			remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT);
		}
	// extension was just disconnected:
	else if(changed & EXTENSION_DISCONNECTED)
		{
#ifdef USE_BEEPS_AND_DELAYS
		Beep(200, 300);
#endif
		// use a non-extension report mode (this gives us back the IR dot sizes)
		remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);
		}
	}




void editGestureName(string name, string newname){
	//Still needs much editing. 

	fstream GestureTrack; //open gestures.txt for input/output
	GestureTrack.open("gestures.txt");
	string GestName;


	while(!GestureTrack.eof()) {
		//Line-by-line: assign compare to the line read.
		getline(GestureTrack,GestName);
		
		//Check if provided name occurs in compare
	//	if(GestName == name + " ");
	
	
	GestureTrack.close();
	}
}



int main(){
	cout << "Load DLLS and Functions into Memory\n";
	
	
	SetConsoleTitle(_T("- Interface: WiiMote Gesture Recognizer"));
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	// create a wiimote object
	wiimote remote;
	// we use a state-change callback to get notified of
	//  extension-related events, and polling for everything else
	remote.ChangedCallback		= on_state_change;
	//  notify us only when the wiimote connected sucessfully, or something
	//   related to extensions changes
	remote.CallbackTriggerFlags = (state_change_flags)(CONNECTED |
													   EXTENSION_CHANGED |
													   MOTIONPLUS_CHANGED);
reconnect:
	COORD pos = { 0, 6 };
	SetConsoleCursorPosition(console, pos);

	// try to connect the first available wiimote in the system
	//  (available means 'installed, and currently Bluetooth-connected'):
	static const TCHAR* wait_str[] = { _T(".  "), _T(".. "), _T("...") };
	unsigned count = 0;
	while(!remote.Connect(wiimote::FIRST_AVAILABLE)) {
		_tprintf(_T("\b\b\b\b%s "), wait_str[count%3]);
		count++;
#ifdef USE_BEEPS_AND_DELAYS
		Beep(500, 30); Sleep(1000);
#endif
		}

#ifdef USE_BEEPS_AND_DELAYS
	Beep(1000, 300); Sleep(2000);
#endif

	
	MessageBox(NULL, "WiiMote Connected!", "Wiimote", MB_OK);

	
	//----------------------Gesture Training Section --------------------------//
		//Load Gestures into the vector of gestures:
	//Define a vector to contain all Gestures,
	vector<Gesture> gestureTemplates;
	//gestureTemplates = importGestures();
	int op = 3;
	bool flag = true;

	do{
		cout << "WELCOME TO THE MAIN MENU!\n"
			 << "-------------------------\n"
			 << "     0 - Make New Gesture\n"
			 << "     1 - Add A Sample to Gesture\n"
			 << "     2 - View Gesture Templates\n"
			 << "     3 - Import Gestures\n" 
			 << "     4 - Write Gestures\n"
			 << "     5 - Score a Gesture\n"
			 << "     6 - EXIT PROGRAM\n\n";

		cout << "Please Enter an Option: ";
		
		cin >> op;
		RecordScore r;
		switch(op)
		{
			case 0:
				gestureTemplates.push_back(makeNewGesture(remote));
				break;
			case 1:
				editGestures(gestureTemplates,remote);
				break;
			case 2:
				printGestures(gestureTemplates);
				break;
			case 3:
				gestureTemplates = importGestures();
				break;
			case 4:
				writeGestures(gestureTemplates);
				break;
			case 5:
				if(gestureTemplates.size() == 0){
					cout << "There are no Templates Loaded...\n";
					system("PAUSE");
					break;
				}

				do{
					r = scoreGesture(gestureTemplates,remote);
					cout << "\nThe Gesture Scored Closest to: "<< r.T.name << endl
						 << "  with a score of " << r.score << " points.\n";
				}while(!remote.Button.B());
				break;
			case 6:
				flag = false;
				break;
		}
		system("CLS");
	}while(flag);

/*
	cout << "\nDo you wish to train a gesture? (Y/N)" << endl;
	
	char ch = 'q';
	while( !(ch == 'Y' || ch =='N') ){

		cin >> ch;
	}
	if( ch == 'Y') makeNewGesture(remote);*/

	//----------------------  End Gesture Training ----------------------------//

	COORD cursor_pos = { 0, 6 };	

#ifdef _ORIGINAL_CODE
	
	//-------------------------Variable Definitions ----------------------------//
	//Define mag variable for Acceleration Magnitude, define MAX for magnitude tuning
	float mag;
	float mag_ine;
	float max = 0; //Max is used for the console tracking of max acceleration in the console feedback
	float theta; //The angle of acceleration between the X & Z components (in radians).
	bool GainRefresh = false; //A switch to decide if the Gain and Frequencies need to be reset.
	bool mag_detect = false;
	float mag_max = 0;
	long start_time = timeGetTime(); //used for logfile management
	float rotation[3][3];     //Define matrix for a 1-2-3 Rotation. This adjusts the frame of reference for the wiimote compared to viewer.
	float A_wii[3] = {0,0,0};  //Accelerations in wii frame and inertial frame, respectively
	float A_ine[3] = {0,0,0}; 
	int i,j; // counters for matrix manipulation
	char type;
	
	

	//Vector Signaling Declarations
	unsigned int time_start;  //Storage for the start point of the vector signal timer
	unsigned int lull_start;  //Storage for the start point of the vector signal lull
	float sum_x=0, sum_z=0;
	float max_x, max_z;
	bool timer_tag = false, lull_tag = false;
	
	ofstream logfile;
	logfile.open("logfile.txt", ios::out | ios::app);  //Opens logfile.txt in output mode and appends all incoming data.
	//Logfile should be formatted to work with Excel's import function.

	logfile << "\n\nWiiMote Tracking Data: "<< timeGetTime() << "\n";
	//logfile << " Time Elapsed, Magnitude,  X Accel,  Z Accel, \n";
	logfile << "X; Y; Z; Time\n";
	//----------------------End Variable Definitions --------------------------//



	//Everything from this point on defines the interaction protocol


	// display the wiimote state data until 'Home' is pressed:
	while(!remote.Button.Home())// && !GetAsyncKeyState(VK_ESCAPE))
		{
		

		// IMPORTANT: the wiimote state needs to be refreshed each pass
		while(remote.RefreshState() == NO_CHANGE)
			Sleep(1); // // don't hog the CPU if nothing changed

		cursor_pos.Y = 0;
		SetConsoleCursorPosition(console, cursor_pos);

		// did we lose the connection?
		if(remote.ConnectionLost())
			{
			_tprintf(
				_T("   *** connection lost! ***                                          \n")
				);
			Beep(100, 1000);
			Sleep(2000);
			COORD pos = { 0, 6 };
			SetConsoleCursorPosition(console, pos);
			goto reconnect;
			}

		// Battery level:
		_tprintf(_T("  Battery: "));
		// (the green/yellow colour ranges are rough guesses - my wiimote
		//  with rechargeable battery pack fails at around 15%)
		(remote.bBatteryDrained	    );
		(remote.BatteryPercent >= 30);
		_tprintf(_T("%3u%%   "), remote.BatteryPercent);

		DWORD current_time = timeGetTime();


		// Rumble
		// Output method:
	    _tprintf( _T("        using %s\n"), (remote.IsUsingHIDwrites()?
											   _T("HID writes") : _T("WriteFile()")));
		
		// 'Unique' IDs (not guaranteed to be unique, check the variable
		//  defintion for details)
		_tprintf(_T("       ID: "));
		_tprintf(_T("%I64u")  , remote.UniqueID);
#ifdef ID2_FROM_DEVICEPATH		// (see comments in header)
		_tprintf(_T("   ID2: "));
		_tprintf(_T("%I64u\n"), remote.UniqueID2);
#else
		_tprintf(_T("\n"));
#endif
		
		//Rotation Matrix calculation
				//Yaw assumed to be zero: If yaw is considered later, replace 0 w/ sin(Yaw) & 1 w/ cos(Yaw)
		//commented out to fix debug issues

				rotation[0][0] = cos(remote.Acceleration.Orientation.Pitch*PI/180)*cos(remote.Acceleration.Orientation.Roll*PI/180);
				rotation[0][1] = 0*cos(remote.Acceleration.Orientation.Roll*PI/180); 
				rotation[0][2] = sin(remote.Acceleration.Orientation.Roll*PI/180);
				rotation[1][0] = sin(remote.Acceleration.Orientation.Roll*PI/180)*sin(remote.Acceleration.Orientation.Pitch*PI/180);
				rotation[1][1] = -0*sin(remote.Acceleration.Orientation.Roll*PI/180)*sin(remote.Acceleration.Orientation.Pitch*PI/180)+1*cos(remote.Acceleration.Orientation.Pitch*PI/180);
				rotation[1][2] = -cos(remote.Acceleration.Orientation.Roll*PI/180)*sin(remote.Acceleration.Orientation.Pitch*PI/180);
				rotation[2][0] = -1*sin(remote.Acceleration.Orientation.Roll*PI/180)*cos(remote.Acceleration.Orientation.Pitch*PI/180)+0*sin(remote.Acceleration.Orientation.Pitch*PI/180);
				rotation[2][1] = 0*sin(remote.Acceleration.Orientation.Roll*PI/180)*cos(remote.Acceleration.Orientation.Pitch*PI/180)+1*sin(remote.Acceleration.Orientation.Pitch*PI/180);
				rotation[2][2] = cos(remote.Acceleration.Orientation.Roll*PI/180)*sin(remote.Acceleration.Orientation.Pitch*PI/180);
		
		//Acceleration Vectors (jerry-rigged Linear Algebra used for the intertial frame of reference)
				A_wii[0] = remote.Acceleration.X;
				A_wii[1] = remote.Acceleration.Y;
				A_wii[2] = remote.Acceleration.Z;

				for(i=0;i<2;i++){
					for(j=0;i<2;j++){
						if(j == 0){
							A_ine[i] = 0;
						}
						A_ine[i] += rotation[i][j]*A_wii[j];
					}
				}
		// Acceleration Display:
		_tprintf(_T("    Accel:"));
		_tprintf(_T("  X %+2.3f  Y %+2.3f  Z %+2.3f  \n"),
					remote.Acceleration.X,
					remote.Acceleration.Y,
					remote.Acceleration.Z);
		

		//mag is calculated as the total magnitude of acceleration in the wii frame of reference
		//   
		mag = sqrt(pow(A_wii[0],2)+pow(A_wii[1],2)+pow(A_wii[2],2));
		mag_ine = sqrt(pow(A_ine[0],2)+pow(A_ine[1],2)+pow(A_ine[2]-1,2)); //Adjust Z for Gravity.

		//Logfile Debug Entry: (Registered immediately after Mag definition)
			logfile << "\n" << remote.Acceleration.X << "; " << remote.Acceleration.Y << "; " << remote.Acceleration.Z << "; " << timeGetTime() - start_time;
			// Feel Free to comment out as desired.
		//logfile << "\n" << timeGetTime() - start_time << ",  " << mag << ",   " << remote.Acceleration.X << ",  " << remote.Acceleration.Z ;

		if(mag > max)
				max = mag;
		_tprintf("  Magnitude = ");
		cout << mag;
		printf("\n   Max = ");
		cout << max << "\n";
		printf("\n");

		//Refresh max if plus button is pressdewww
		if(remote.Button.Plus()) 
				max = 0;

		// Orientation estimate (shown red if last valid update is aging):
		_tprintf(_T("   Orient:"));
		_tprintf(_T("  UpdateAge %3u  "), remote.Acceleration.Orientation.UpdateAge);


		_tprintf("\n\nStandard Wii Orientations\n");
		_tprintf(_T("Pitch:%4ddeg  Roll:%4ddeg  \n"),
				 (int)remote.Acceleration.Orientation.Pitch,
				 (int)remote.Acceleration.Orientation.Roll );

		//Motion Plus Feedback Section
		_tprintf("\n\nMotion Plus Functions");
		_tprintf("\n");
		_tprintf(_T("      Raw: "));
		_tprintf(_T("Yaw: %04hx  ")   , remote.MotionPlus.Raw.Yaw);
		_tprintf(_T("Pitch: %04hx  ") , remote.MotionPlus.Raw.Pitch);
		_tprintf(_T("Roll: %04hx  \n"), remote.MotionPlus.Raw.Roll);
		_tprintf(_T("    Float: "));
		_tprintf(_T(" %8.3fdeg")     , remote.MotionPlus.Speed.Yaw);
		_tprintf(_T("  %8.3fdeg")   , remote.MotionPlus.Speed.Pitch);
		_tprintf(_T(" %8.3fdeg \n")   , remote.MotionPlus.Speed.Roll);

	//----------------End Wiimote Feedback Console----------------------//

		


	//----------------Begin Acceleration Signaling----------------------//

		//Note: when accelerating the remote in one direction, you must eventually exert a COUNTER directional force.
		// This accelerates the remote in the *opposite* direction, causing an opposite signal. 
		// This must be accounted for and filtered out.
		// What is attempted here is a timer based Jerry-Rigged system. We collect data for CONST_TIMER
		// and ignore the CONST_LULL period of time immediately after. 
	



		}
		
		

	
#endif	
	// disconnect (auto-happens on wiimote destruction anyway, but let's play nice)
	remote.Disconnect();
	Beep(1000, 200);

	// for automatic 'press any key to continue' msg
	cout << "Wiimote Disconnected...\n"; 
	system("PAUSE");
	CloseHandle(console);
	return 0;
}

Gesture makeNewGesture(wiimote &remote)
{
	
	Gesture newGesture;
	char name[16];
	cout << "\n\nGesture Creator: \n";
	while(1){
		cout << "Enter a Name for the Gesture: ";
		cin >> name;
		newGesture.name = name;
		newGesture.samples = 1;
		cout << "Is the Gesture directional? (Y/N) ";
	 
		char ch = 'q';
		while( !(ch =='Y' || ch =='N'))
			cin >> ch;

		if(ch == 'Y') newGesture.directionality = true;
		if(ch == 'N') newGesture.directionality = false;
	
		cout << "\nThe new gesture is as follows: "<<
				"\n   -      Name     : " << name << 
				"\n   - Directionality: " << newGesture.directionality << endl;

		cout <<"\nIs this acceptable? (Y/N) ";

		ch='q';
		while(!(ch == 'Y' || ch=='N'))
			cin >> ch;
		if(ch == 'Y') break;
	}
	
	// Normalize the Data
	vector<Vector2D> points = recordSignal(remote);
	points = resample(points, NUMPOINTS);
	
	
	points = scaleTo(points, WINDOW);
	Vector2D origin = Vector2D(0,0);
	points = translateTo(points, origin);

	

	newGesture.data = points;


/*	cout << "\n\nWould you like to add another sample? (Y/N)";
	char ch3 = 'q';
	cin >> ch3;
	while(ch3 == 'Y' || ch3 == 'y')
	{
		newGesture = addSampleData(newGesture, remote);
		cout << "\n\nWould you like to add another sample? (Y/N)";
		cin >> ch3;
	}
	*/
	return newGesture;
}

Gesture addSampleData(Gesture T, wiimote &remote)
{
	vector<Vector2D> points = recordSignal(remote);
	points = resample(points,NUMPOINTS);
	points = scaleTo(points, WINDOW);
	Vector2D origin = Vector2D(0,0);
	points = translateTo(points, origin);

	T.samples++;




	for(int i=0; i < (int) points.size(); i++){
		//T.data.at(i).x = points.at(i).x;
		//T.data.at(i).y = points.at(i).y;
		T.data.at(i).x = (float)((T.data.at(i).x * T.samples-1)/ T.samples) + (points.at(i).x / T.samples);
		T.data.at(i).y = (float)((T.data.at(i).y * T.samples-1)/ T.samples) + (points.at(i).y /T.samples);
	}
	return T;
	// If the Gesture T is not updated will "return T;"
}


/*
 *Gestures.txt
 *---------------------
 * Up           <- Name of Gesture
 * 0            <- Directionality 1-true 0-false
 * 0.132 0.472  <- X-Cord / Z-Cord
 * 0.135 0.828
 * ...
 * 32           <- Sample Size
 */

// WRITE OUT NEW GESTURES
void writeGestures(vector<Gesture> gestureList)
{
	ofstream outFile;
	outFile.open("gestures.txt"); //Open the gesture.txt repository for addendum

	for(int i = 0; i < (int) gestureList.size(); i++)
	{
		outFile << gestureList.at(i).name << endl;
		outFile << gestureList.at(i).directionality << endl;
		for(int j = 0; j < (int) gestureList.at(i).data.size(); j++)
		{
			outFile << gestureList[i].data[j].x << " "
				   << gestureList[i].data[j].y << endl;
		}
		outFile << gestureList.at(i).samples;
		if( i < (int) gestureList.size()-1)
			outFile << endl;
	}
	outFile.close();
}

// READ IN GESTURES
vector<Gesture> importGestures()
{
	

	Gesture newGesture;
	vector<Gesture> gestureList;
	int numDataPoints = NUMPOINTS;
	
	char name[16];
	float flX = 0.0f;
	float flY = 0.0f;
	
	ifstream inFile	("gestures.txt", ifstream::in);
	// While the file stream is good 
	while(!inFile.eof()) //(replacable with: !inFile.eof())
	{
		newGesture.data.clear();

		inFile >> newGesture.name;
		cout << "Loading Gesture: " << newGesture.name << endl;
		// MAY NEED TO REPLACE THIS TO RETREIVE ACTUAL BOOLEAN VALUES

		inFile >> newGesture.directionality; 
		Vector2D q;
		for(int i=0; i < numDataPoints; i++)
		{
			inFile >> flX;
			q.x = flX;
			inFile >> flY;
			q.y = flY;
			newGesture.data.push_back(q);
		}
		inFile >> newGesture.samples;

		gestureList.push_back(newGesture);
	}
	inFile.close();

	return gestureList;
}

// READ DATA DIRECTLY FROM WIIMOTE  
vector<Vector2D> recordSignal(wiimote &remote)
{
	bool timer_break = true;
	int start_time;
	unsigned count = 0;
	vector<Vector2D> points;

	float rotation[3][3];     //Define matrix for a 1-2-3 Rotation. This adjusts the frame of reference for the wiimote compared to viewer.
	float A_wii[3] = {0,0,0};  //Accelerations in wii frame and inertial frame, respectively
	float A_ine[3] = {0,0,0}; 
	int i,j; // counters for matrix manipulation

	rotation[0][0] = cos(remote.Acceleration.Orientation.Pitch*PI/180)*cos(remote.Acceleration.Orientation.Roll*PI/180);
	rotation[0][1] = 0*cos(remote.Acceleration.Orientation.Roll*PI/180); 
	rotation[0][2] = sin(remote.Acceleration.Orientation.Roll*PI/180);
	rotation[1][0] = sin(remote.Acceleration.Orientation.Roll*PI/180)*sin(remote.Acceleration.Orientation.Pitch*PI/180);
	rotation[1][1] = -0*sin(remote.Acceleration.Orientation.Roll*PI/180)*sin(remote.Acceleration.Orientation.Pitch*PI/180)+1*cos(remote.Acceleration.Orientation.Pitch*PI/180);
	rotation[1][2] = -cos(remote.Acceleration.Orientation.Roll*PI/180)*sin(remote.Acceleration.Orientation.Pitch*PI/180);
	rotation[2][0] = -1*sin(remote.Acceleration.Orientation.Roll*PI/180)*cos(remote.Acceleration.Orientation.Pitch*PI/180)+0*sin(remote.Acceleration.Orientation.Pitch*PI/180);
	rotation[2][1] = 0*sin(remote.Acceleration.Orientation.Roll*PI/180)*cos(remote.Acceleration.Orientation.Pitch*PI/180)+1*sin(remote.Acceleration.Orientation.Pitch*PI/180);
	rotation[2][2] = cos(remote.Acceleration.Orientation.Roll*PI/180)*sin(remote.Acceleration.Orientation.Pitch*PI/180);
		
	//Acceleration Vectors (jerry-rigged Linear Algebra used for the intertial frame of reference)
	





	cout << "\nHold 'A' to Begin Recording Signal...\n";
	Sleep(150);

	//Press 'A' and Break Threshold 
	do{
		while(remote.RefreshState() == NO_CHANGE)
			Sleep(1);
	}while(!remote.Button.A()); //&& sqrt(remote.Acceleration.X*remote.Acceleration.X+
								//	remote.Acceleration.Y*remote.Acceleration.Y+
								//	remote.Acceleration.Z*remote.Acceleration.Z) < A_THRESH);

	cout << "Start Collecting Data!";
	Vector2D q;
	while(remote.Button.A())
	{
		while(remote.RefreshState() == NO_CHANGE)
			Sleep(1);

		A_wii[0] = remote.Acceleration.X;
		A_wii[1] = remote.Acceleration.Y;
		A_wii[2] = remote.Acceleration.Z;

		for(i=0;i<=2;i++)
		{
			for(j=0;j<=2;j++)
			{
				if(j == 0)
				{
					A_ine[i] = 0;
				}
				A_ine[i] += rotation[i][j]*A_wii[j];
				//Sleep(1);
			}
		}
		
		q.x = A_ine[0];
		q.y = A_ine[2];
		points.push_back(q);
		
		//start_time = timeGetTime();

		//if(sqrt(remote.Acceleration.X*remote.Acceleration.X+
		//		remote.Acceleration.Y*remote.Acceleration.Y+
		//		remote.Acceleration.Z*remote.Acceleration.Z) <= A_THRESH)
		//{ 
		//	 if((timeGetTime() - start_time) >= 10)
		//		 timer_break = false;
		//}
	}

	return points;
}

RecordScore scoreGesture(vector<Gesture> templates, wiimote &remote)
{
	vector<Vector2D> points;
	RecordScore score;

	points = recordSignal(remote);

	points = resample(points,NUMPOINTS);
	points = scaleTo(points, WINDOW);
	Vector2D origin = Vector2D(0,0);
	points = translateTo(points, origin);
	
	score = recognize(points, templates, WINDOW);
	return score;
}

void printGestures(vector<Gesture> gestureList)
{
	system("CLS");
	for(int i=0;i<(int)gestureList.size();i++)
	{
		cout << gestureList[i].name << endl;
		cout << "----------------------------------" << endl;
		cout << "Directionality: " << gestureList[i].directionality << endl;
//		cout << "  X-Acceleration / Z-Accelaration \n";
//		for(int j=0; j< (int) gestureList.at(i).data.size(); j++)
//			cout << "     "<< gestureList[i].data[j].x <<"  /  "<< gestureList[i].data[j].y << endl;
		cout << "Number of Samples: "<< gestureList[i].samples << endl;
		cout << "----------------------------------" << endl;
		
	}
	system("PAUSE");
}

void editGestures(vector<Gesture> &gestureList, wiimote &remote)
{
	cout <<	" Which Gesture would you like to edit?\n";
	for(int i = 0; i < (int) gestureList.size(); i++)
		cout << i <<". "<< gestureList[i].name << endl;

	int numGest;
	cin >> numGest;

	do{
		gestureList[numGest] = addSampleData(gestureList[numGest], remote);
	}while(!remote.Button.B());
}