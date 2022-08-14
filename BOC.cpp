/*
	Jai Maa Bhavani
	Game: Board of Conquest
*/

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H> 
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

using namespace std;


class Point{   
public:
	int x, y;
	
	bool check_adjacency(Point p){
		int a = (p.x-x)*(p.x-x)+(p.y-y)*(p.y-y);
		if(a==1 or a==2) return true;
		return false;
	}
	
	bool check_similarity(Point p){
		return ((x==p.x)&&(y==p.y));
	}
};


class Tile : public Fl_Button{ //Abstract Tile class.
protected:
	int x, y, w, h;
public:
	Point tile_id;
	int score;
	
	Tile(int x, int y, int w, int h, const char* str) : Fl_Button(x,y,w,h,str){
		this->x=x; this->y=y;
		this->w=w; this->h=h;
		score = 0;
	}
	
};


string s[64];   

void cb_tile(Tile *w){
	ostringstream oss;
	oss.str("");
	oss << w->score;
	
	int x = w->tile_id.x;
	int y = w->tile_id.y;
	int a = (x-1)*8 + y - 1;
		
	s[a] = (oss.str());
	cout << "Updating score: " <<  w->tile_id.x << " " << w->tile_id.y <<" " << w->score << endl;
	if(w->score != 0) w->label(s[a].c_str());
	else w->label("");

}	
	
	
class Player{
private:
	int id;
	Fl_Color col;
	
public: 
	Tile *Progress;
	vector<Point> tile_id; // vector of tiles.
	int lands; //no of occupied tiles.
	vector<Tile*> tiles;
	
	void add_base(Point tile, Fl_Color c, Tile* t, int i){
		tile_id.push_back(tile);
		lands++;
		tiles.push_back(t);
		col = c;
		id = i;
		cout << "Player " << id << " sets its base at: " << tiles[0]->tile_id.x << "," << tiles[0]->tile_id.y << endl;
	}
	
	void add_land(Point tile, Tile* t){
		tile_id.push_back(tile);
		lands++;
		tiles.push_back(t);
		cout << "Player " << id << " conquers land at: " << t->tile_id.x << ", " << t->tile_id.y << endl;
	}
	
	int find_index(Point p){
		int i;
		for(i=0;i<tile_id.size();i++){
			if(p.check_similarity(tile_id[i])) break;
		}
		return i;
	}
	
	void remove_land(Point p){
		int index = find_index(p);
		tile_id.erase(tile_id.begin() + index);
		tiles.erase(tiles.begin() + index);	
		lands--;
	}
		
	Fl_Color get_color(){   //have to use it in warring situations
		return col;
	}
	
	vector<Tile*> backup_from_vicinity(Point &p){
		vector <Tile*> backup_tiles_ptr;
		for(int i=0; i<tile_id.size();i++){
			if(p.check_adjacency(tile_id[i])){
				backup_tiles_ptr.push_back(tiles[i]);
			}
		}
		return backup_tiles_ptr;
	}	
	
	int get_id(){
		return id;
	}
	
	void declare_victory(){
		if(id==1) Progress->label("RED Wins. Congo!");
		else Progress->label("BLUE Wins. Congo!");
	}
	
	void change_label(){
		if(id==2) Progress->label("RED's Turn........");
		else Progress->label("BLUE's Turn........");
	}
	
};

Player P[2]; //Globally declaring two players

class Operational_Tile : public Tile{  //General Base Tile.
private:
	int type;
public:
	Operational_Tile(int x, int y, int w, int h, int type, const char* str) : Tile(x,y,w,h,str){
		this->type = type;
		color(FL_WHITE);
	}

	virtual int handle(int e){
		if(e == FL_PUSH && type==2){
			cout << "Alright, Bye! \n" << endl;
			exit(1);
		}
		return 1;
	}
	
};

int chance = 0;

class Gaming_Tile : public Tile{    //The 64 tiles involved in the board game.
private:
	Fl_Color col; 
	Player* king;
	
public:
	
	Gaming_Tile(int x, int y, int w, int h, const char* str) : Tile(x,y,w,h,str){
		col = FL_GREEN;
		tile_id.x= (x+50)/100;
		tile_id.y = (y+50)/100;
		king = NULL;
		color(col);
	}
	
	void change_color(Fl_Color C){  //changing gaming tiles colour when conquered or lost.
		redraw();
		color(C);
	}
	
	int handle(int e){
		if(e == FL_PUSH){
		
			vector<Tile* > back_up_tile_ptrs = P[chance%2].backup_from_vicinity(tile_id);
			P[chance%2].change_label();
			
			if(back_up_tile_ptrs.size()!=0){
				cout << "There are " << back_up_tile_ptrs.size() << " friend base(s) for backup" << endl;
				
				if(col == P[chance%2].get_color()){
					
					cout << tile_id.x << ", " << tile_id.y << " already under reign of ";
					if(col==FL_RED) cout << "RED \n";
					else cout << "BLUE \n";
					
					int backup = 0;
					for(int i=0; i<back_up_tile_ptrs.size();i++){
						int a = back_up_tile_ptrs[i]->score;
						backup += a/2;					
						back_up_tile_ptrs[i]->score = a - a/2;
						
						cb_tile(back_up_tile_ptrs[i]);
					}
					score+=backup;
					cout << "Receiving backup of: " << backup << endl;
					
					cb_tile(this);					
				}
				
				else if(col == P[(chance+1)%2].get_color()){
					
					cout << tile_id.x << ", " << tile_id.y;
					if(col==FL_RED) cout << " BLUE attacks RED\n";
					else cout << " RED attacks BLUE\n";
					
					int deploy = 0;
					for(int i=0; i<back_up_tile_ptrs.size();i++){
						int a = back_up_tile_ptrs[i]->score;
						deploy += a/2;					
						back_up_tile_ptrs[i]->score = a - a/2;
						
						cb_tile(back_up_tile_ptrs[i]);
					}
					
					cout << "Attack with might of: " << deploy << endl;
					
					if(deploy > score){
						col = P[chance%2].get_color();
						king->remove_land(tile_id); //Have to implement this in class Player. - this is the problem
						king = &P[chance%2];
						score = deploy - score;
						king->add_land(tile_id,this);
					
						change_color(col);
						
						cout << "Attack successful " << endl;
					}
					else if(deploy == score){ 
						col = FL_GREEN;
						king->remove_land(tile_id);
						king = NULL;
						score = 0;
						
						change_color(col);
						
						cout << "Attack balanced " << endl;
					}
					else{  // working fine
						score = score - deploy;
						cout << "Attack failed " << endl;
					}
						
					cb_tile(this);
				}
				
				else{				
					col = P[chance%2].get_color(); 
					king = &P[chance%2];
					king->add_land(tile_id,this);
					
					cout << tile_id.x << ", " << tile_id.y << " now under reign of the ";
					if(col==FL_RED) cout << "RED \n";
					else cout << "BLUE \n";
					
					int backup = 0;
					for(int i=0; i<back_up_tile_ptrs.size();i++){
						int a = back_up_tile_ptrs[i]->score;
						backup += a/2;					
						back_up_tile_ptrs[i]->score = a - a/2;
						
						cb_tile(back_up_tile_ptrs[i]);
					}
					score+=backup;
					cout << "Receiving backup of: " << backup << endl;
					
					cb_tile(this);
					
					change_color(col);
				}
			}
			
			if(P[(chance+1)%2].lands==0){
				P[chance%2].declare_victory();
				cout << "Player " << chance%2+1 << " wins" << endl;
			}
			
			if(chance%2==1){
				for(int i=0; i<P[0].tiles.size(); i++){
					(P[0].tiles[i]->score)+=(3+P[0].lands);
					cb_tile(P[0].tiles[i]);
				}
				for(int i=0; i<P[1].tiles.size(); i++){
					(P[1].tiles[i]->score)+=(3+P[1].lands);
					cb_tile(P[1].tiles[i]);
				}
			cout << "Score Updated: All " << P[0].lands << " lands of Player 1" << endl;	
			cout << "Score Updated: All " << P[1].lands << " lands of Player 2" << endl;			
			}
		

			chance++; 
			
					
		}
		return 1;
	}
	
	void set_king_base(int player_no){
		switch(player_no){
			case 1: king = &P[0];
				col = FL_RED;
				break;
			case 2: king = &P[1];
				col = FL_BLUE;
			default: break;
		}
		king->add_base(tile_id,col,this,player_no);
		score = 20;
		color(col);
		label("20");
		cout << "Base created for Player " << king->get_id() << " and initiated score " << atoi(label()) << endl; 
	}
			
};

static void WinQuit(Fl_Widget* w, void* userdata){
	cout << "Are you sure you wanna Quit?, coz I won't let you!" << endl;
}

int main(int argc, char *argv[]) {
	
	Fl_Window* w = new Fl_Window(900,900,"Sachi Games Studio");
	Gaming_Tile* t[8][8];
	for(int i=0; i<8; i++){
		for(int j=0; j<8; j++){
			t[i][j] = new Gaming_Tile(50+i*100, 50+j*100, 100, 100,"");
		}
	}
	
	Operational_Tile start(20,860,220,30,1,"RED's Turn........");
	Operational_Tile quit(780,860,100,30,2,"Quit");
	Operational_Tile heading(325,0,250,40,3,"Board of Conquest");
	
	start.align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
	start.labelsize(20);
	start.labelfont(FL_TIMES_BOLD_ITALIC);
	heading.labelsize(20);
	heading.labelfont(FL_TIMES_BOLD_ITALIC);
	quit.labelsize(20);
	quit.labelfont(FL_TIMES_BOLD_ITALIC);
	
	cout << "THE NEWS HERALD" << endl;
	
	t[0][0]->set_king_base(1);
	t[7][7]->set_king_base(2);
	P[0].Progress=&(start);
	P[1].Progress=&(start);
	
	
	w->color(FL_BLACK);
	w->show();

   	w->callback(WinQuit);
 	
   	return Fl::run();	
    
}
