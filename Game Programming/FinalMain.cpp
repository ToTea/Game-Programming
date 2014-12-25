/*==============================================================
  character movement testing using Fly2

  - Load a scene
  - Generate a terrain object
  - Load a character
  - Control a character to move
  - Change poses

  (C)2012 Chuan-Chang Wang, All Rights Reserved
  Created : 0802, 2012

  Last Updated : 1010, 2014, Kevin C. Wang
 ===============================================================*/
#include "FlyWin32.h"
#define PI 3.14159265
#define enemySize 6
float bug=-2.0f;
float bug2=-2.0f;
float bug3=-2.0f;
float bug4=-2.0f;
float bug5=-2.0f;
float bug6=-2.0f;

VIEWPORTid vID;                 // the major viewport
SCENEid sID;                    // the 3D scene
OBJECTid cID, tID;              // the main camera and the terrain for terrain following
ROOMid terrainRoomID = FAILED_ID;
TEXTid textID = FAILED_ID;
FnScene scene;
FnObject terrain;
BOOL4 beOK;

float testAngle=-2.0f;//�ҥ~����

float GetDistance(float*pos1,float*pos2){
	return (sqrt(pow(pos1[0] - pos2[0], 2) + pow(pos1[1] - pos2[1], 2) +pow(pos1[2] - pos2[2], 2)));
}

//���o�⨤�⤧���Z��
float GetDistanceWithCharacterID(int a1, int a2) {
	FnCharacter actorLocal;
	actorLocal.ID(a1);
	FnCharacter enemy;
	enemy.ID(a2);
	float actor_pos[3], enemy_pos[3], distance;

	actorLocal.GetPosition(actor_pos);
	enemy.GetPosition(enemy_pos);

	return GetDistance(actor_pos,enemy_pos);

}

//��Ĩ��Q�����ɥi�ϥΦ��禡��L���ӭ��諸��V
void getResultFdir(int a1, int a2, float *ary) {
	FnCharacter actorLocal;
	actorLocal.ID(a1);
	FnCharacter enemy;
	enemy.ID(a2);
	float resultFdir[3],actor2Pos[3],actor1Pos[3];
	actorLocal.GetPosition(actor1Pos);
	enemy.GetPosition(actor2Pos);
	resultFdir[0]=actor1Pos[0]-actor2Pos[0];
	resultFdir[1]=actor1Pos[1]-actor2Pos[1];
	resultFdir[2]=actor1Pos[2]-actor2Pos[2];
	
	ary[0]=resultFdir[0];
	ary[1]=resultFdir[1];
	ary[2]=resultFdir[2];
}

//�����p�⾹�Abug�w�ץ��A�U�c��float�ȹB��~�t��cross�Ȧb�p���I�Un��>1��<-1�]���R�XNaN = =
float getAngle(float* fDir0,float*fDir1,bool Zflag){
	
		float angle,cross,lengthA,lengthB,toACOS;

		if(Zflag){
			cross=fDir0[0]*fDir1[0]+fDir0[1]*fDir1[1]+fDir0[2]*fDir1[2];
			lengthA=sqrt(pow(fDir0[0],2)+pow(fDir0[1],2)+pow(fDir0[2],2));
			lengthB=sqrt(pow(fDir1[0],2)+pow(fDir1[1],2)+pow(fDir1[2],2));
		}else{
			cross=fDir0[0]*fDir1[0]+fDir0[1]*fDir1[1];
			lengthA=sqrt(pow(fDir0[0],2)+pow(fDir0[1],2));
			lengthB=sqrt(pow(fDir1[0],2)+pow(fDir1[1],2));
		}

		if((lengthA>0)&&(lengthB>0)){
		
			toACOS=(cross/lengthA/lengthB);
			if(toACOS>1.0f){
				toACOS=1.0f;
			}else if(toACOS<-1.0f){
				toACOS=-1.0f;
			}

			angle=(float)acos(toACOS)*180.0/PI;
			return angle;

		}else{
			return 0.0f;
		}
}

//���o�⨤�⤧������
float getAngleWithCharacterID(int a1, int a2,bool Zflag) {
	FnCharacter actorLocal;
	actorLocal.ID(a1);
	FnCharacter enemy;
	enemy.ID(a2);
	float actor1Fdir[3];
	float resultFdir[3];

	getResultFdir(a2,a1,resultFdir);

	actorLocal.GetDirection(actor1Fdir,NULL);

	return getAngle(resultFdir,actor1Fdir,Zflag);
	
}

//���լO�_��e�i�A �]�A�I��P�I�H�A�I�H�i�]�w�Z��
bool testIFforward(int a1, int a2,float distanceLimit){
	FnCharacter actorLocal;
	bool FlagLocal;
	int walkFlag;
	int previousDistance=GetDistanceWithCharacterID(a1, a2);

	actorLocal.ID(a1);
	walkFlag=actorLocal.MoveForward(10.0f, TRUE, FALSE, FALSE, FALSE);

	if(walkFlag==WALK){
		if(previousDistance<=distanceLimit){
			if(GetDistanceWithCharacterID(a1, a2)<=previousDistance){
				actorLocal.MoveForward(-10.0f, TRUE, FALSE, FALSE, FALSE);
				return false;
			}else{
				actorLocal.MoveForward(-10.0f, TRUE, FALSE, FALSE, FALSE);
				return true;
			}
		}else{
			actorLocal.MoveForward(-10.0f, TRUE, FALSE, FALSE, FALSE);
			return true;
		}
	}
	else{
		return false;
	}
}

//����A�޲z����X�Ъ����P�_
class Controller{
	public:
		Controller(){
			upArrow=false;
			downArrow=false;
			leftArrow=false;
			rightArrow=false;
			Wflag=false;
			Aflag=false;
			Sflag=false;
			Dflag=false;
			moveDirectionFlag=-1;
			cameraDirectionFlagH=-1;
			cameraDirectionFlagV=-1;
		}
		//�]�wPlayer���ʩMCamera����������ʪ��X��
		void setFlags(){
			setMoveDirectionFlag();
			setCameraDirectionFlagV();
			setCameraDirectionFlagH();
		}

		//�]�wPlayer����flag
		void setMoveDirectionFlag(){
			if(getW()&&!getS()&&!getA()&&!getD()){
				moveDirectionFlag=0;
			}else if(!getW()&&getS()&&!getA()&&!getD()){
				moveDirectionFlag=1;
			}else if(!getW()&&!getS()&&getA()&&!getD()){
				moveDirectionFlag=2;
			}else if(!getW()&&!getS()&&!getA()&&getD()){
				moveDirectionFlag=3;
			}else if(getW()&&!getS()&&getA()&&!getD()){
				moveDirectionFlag=4;
			}else if(!getW()&&getS()&&getA()&&!getD()){
				moveDirectionFlag=5;
			}else if(getW()&&!getS()&&!getA()&&getD()){
				moveDirectionFlag=6;
			}else if(!getW()&&getS()&&!getA()&getD()){
				moveDirectionFlag=7;
			}else{
				moveDirectionFlag=-1;
			}
			
		}

		//�]�wCamera�������flag
		void setCameraDirectionFlagV(){
			if((getUpArrow())&&(!getDownArrow())){
				cameraDirectionFlagV=0;
			}else if((!getUpArrow())&&(getDownArrow())){
				cameraDirectionFlagV=1;
			}else{
				cameraDirectionFlagV=-1;
			}
		}

		//�]�wCamera�������flag
		void setCameraDirectionFlagH(){
			if((getLeftArrow())&&(!getRightArrow())){
				cameraDirectionFlagH=0;
			}else if((!getLeftArrow())&&(getRightArrow())){
				cameraDirectionFlagH=1;
			}else{
				cameraDirectionFlagH=-1;
			}
		}

		//flag�}���Pget method
		int getMoveDirectionFlag(){
			return moveDirectionFlag;
		}

		int getCameraDirectionFlagV(){
			return cameraDirectionFlagV;
		}

		int getCameraDirectionFlagH(){
			return cameraDirectionFlagH;
		}

		void WOn(){
			Wflag=true;
		}

		void WOff(){
			Wflag=false;
		}

		bool getW(){
			return Wflag;
		}

		void AOn(){
			Aflag=true;
		}

		void AOff(){
			Aflag=false;
		}

		bool getA(){
			return Aflag;
		}

		void SOn(){
			Sflag=true;
		}

		void SOff(){
			Sflag=false;
		}

		bool getS(){
			return Sflag;
		}

		void DOn(){
			Dflag=true;
		}

		void DOff(){
			Dflag=false;
		}

		bool getD(){
			return Dflag;
		}
		
		void upArrowOn(){
			upArrow=true;
		}

		void upArrowOff(){
			upArrow=false;
		}

		bool getUpArrow(){
			return upArrow;
		}

		void downArrowOn(){
			downArrow=true;
		}

		void downArrowOff(){
			downArrow=false;
		}

		bool getDownArrow(){
			return downArrow;
		}

		void leftArrowOn(){
			leftArrow=true;
		}

		void leftArrowOff(){
			leftArrow=false;
		}

		bool getLeftArrow(){
			return leftArrow;
		}

		void rightArrowOn(){
			rightArrow=true;
		}

		void rightArrowOff(){
			rightArrow=false;
		}

		bool getRightArrow(){
			return rightArrow;
		}

	private:
		bool upArrow;
		bool downArrow;
		bool leftArrow;
		bool rightArrow;
		bool Wflag;
		bool Aflag;
		bool Sflag;
		bool Dflag;
		int moveDirectionFlag;
		int cameraDirectionFlagV;
		int cameraDirectionFlagH;
};

class enemy { 
public: 
	enemy(CHARACTERid donzoID,CHARACTERid playerID,char* name,float*pos_c,float*fDir_c,float*uDir_c,float turnSpeed_input,float walkSpeed_input,float toTargetRange_input,int HP_input,int index_input){
		//��l��
		actorID_c = scene.LoadCharacter(name);
		
        actor_c.ID(actorID_c);
		actor_c.SetDirection(fDir_c, uDir_c);
		actor_c.SetTerrainRoom(terrainRoomID, 10.0f);
        beOK = actor_c.PutOnTerrain(pos_c);

		if(strcmp(name,"Donzo2")==0){
			enemy_category=0;
			hitCounter=45;//�w��
			idleID_c = actor_c.GetBodyAction(NULL, "Idle");
			hurtID_c = actor_c.GetBodyAction(NULL, "DamageH");
			dieID_c = actor_c.GetBodyAction(NULL, "Die");
			runID_c = actor_c.GetBodyAction(NULL, "Run");
			attack1ID_c=actor_c.GetBodyAction(NULL, "AttackL2");
		}else if(strcmp(name,"Robber02")==0){
		    enemy_category=1;
			hitCounter=120;//�w��
			idleID_c = actor_c.GetBodyAction(NULL, "CombatIdle");
			hurtID_c = actor_c.GetBodyAction(NULL, "Damage2");
			dieID_c = actor_c.GetBodyAction(NULL, "Die");
			runID_c = actor_c.GetBodyAction(NULL, "Run");
			attack1ID_c=actor_c.GetBodyAction(NULL, "NormalAttack1");
		}else if(strcmp(name,"Lyubu2")==0){
		    enemy_category=2;
			hitCounter=120;//�w��
			idleID_c = actor_c.GetBodyAction(NULL, "Idle");
			hurtID_c = actor_c.GetBodyAction(NULL, "LeftDamaged");
			dieID_c = actor_c.GetBodyAction(NULL, "Die");
			runID_c = actor_c.GetBodyAction(NULL, "Run");
			attack1ID_c=actor_c.GetBodyAction(NULL, "NormalAttack1");
		}
		HP=HP_input;


		curPoseID_c = idleID_c;
		actor_c.SetCurrentAction(NULL, 0, curPoseID_c);
		actor_c.Play(START, 0.0f, FALSE, TRUE);
		actor_c.TurnRight(90.0f);

		turnRLflag=-1;
		turnSpeed=turnSpeed_input;
		timeCounter=-1;
		HP=HP_input;
		index=index_input;
		walkSpeed=walkSpeed_input;
		toTargetRange=toTargetRange_input;
		blockCounter=0;
		playerID_c=playerID;
		donzoID_c=donzoID;
		
		damageToPlayer=0;
		for(int y=0;y<enemySize;y++){
			damageToEnemies[y]=0;
		}
		//init damage to others

		if(index!=0){
			targetID_c=donzoID_c;
		}else{
			targetID_c=-1;
		}
		//init first target

		savedTurnTarget[0]=-9999.0;
		savedTurnTarget[1]=-9999.0;
		savedTurnTarget[2]=-9999.0;
	} 


	//GameAI call this
	void doActions(int skip,int targetHP,CHARACTERid firstAttackerID,int totalDamage){
		targetHP_c=targetHP;
		
		float localPos[3];
		actor_c.GetPosition(localPos);
		
		if((localPos[0]!=-99999.0)&&(localPos[1]!=-99999.0)&&(localPos[2]!=-99999.0)){

			beHit(firstAttackerID,totalDamage);
			
			//�����P�w�o�ͦb(hitCounter)�A�������o�ͦb(-1)
			if(timeCounter!=-1){
					if(timeCounter==hitCounter){
						if(curPoseID_c==attack1ID_c){
							attackHit(1);
						}
					}
					timeCounter--;
			}
		
			if((curPoseID_c==idleID_c)||(curPoseID_c==runID_c)){
			
				actor_c.Play(LOOP, (float) skip, FALSE, TRUE);
			
				if(targetHP_c>0){
					if(index!=0){
						//��blockCounter���Q�]�w��>0�h�i����V�]�w�B��V�B�]�VPlayer�P����
						if(blockCounter<=0){
							turnSetting();
							turn();
							runAndAttack();
						}else{
							findRoute();
							//�Ϥ��M��
						}
					}else{
							turnSetting();
							turn();
							if((GetDistanceWithCharacterID(actorID_c,targetID_c)<=toTargetRange)&&(turnRLflag==-1)){
								attack();
							}
					}
					//Donzo's action,he can't run
				}
				
			}else{
				BOOL4 playOver=actor_c.Play(ONCE, (float) skip, FALSE, TRUE);

				if (playOver == FALSE && curPoseID_c != dieID_c){
					curPoseID_c = idleID_c;
					actor_c.SetCurrentAction(NULL, 0, curPoseID_c);
				}
			}
		}
	}

	CHARACTERid getID(){
		return actorID_c;
	}

	CHARACTERid getTargetID(){
		return targetID_c;
	}

	int getHP(){
		return HP;
	}

	//���~�����D�R���æ^�Ƕˮ`
	int ifHitPlayer(){
		int temp=damageToPlayer;
		damageToPlayer=0;
		return temp;
	}

	int ifHitEnemy(int index){
		int temp=damageToEnemies[index];
		damageToEnemies[index]=0;
		return temp;
	}

	//init enemies ID
	void setEnemiesID(CHARACTERid*enemiesID_input){
		enemiesID=enemiesID_input;
	}
    
private:
	int enemy_category;
	FnCharacter actor_c;
	ACTIONid idleID_c,curPoseID_c,dieID_c,hurtID_c,runID_c,attack1ID_c;
	CHARACTERid actorID_c;
	CHARACTERid targetID_c;
	CHARACTERid playerID_c;
	CHARACTERid donzoID_c;
	int targetHP_c;
	float turnSpeed;
	float savedTurnTarget[3];
	int turnRLflag;
	int timeCounter;
	int hitCounter;
	int HP;
	int index;
	float walkSpeed;
	float toTargetRange;
	int damageToPlayer;
	int damageToEnemies[enemySize];
	CHARACTERid* enemiesID;
	int blockCounter;
	bool blockTurning;

	//�P�_�O�_�Q�����ó]�w�����ʧ@
	void beHit(CHARACTERid firstAttackerID,int totalDamage){

			float resultFdir[3];
			float fdir[3],udir[3];
		
			if(totalDamage>0){	
				targetID_c=firstAttackerID;

				//�Q�����N���m�����ʧ@counter
				timeCounter=-1;	
				
				if(HP>0){
					HP-=totalDamage;
					getResultFdir(firstAttackerID,actorID_c,resultFdir);
					actor_c.GetDirection(fdir,udir);
					resultFdir[2]=fdir[2];
					actor_c.SetDirection(resultFdir,udir);
				}

				if(HP>0){
					curPoseID_c = hurtID_c;
					actor_c.SetCurrentAction(NULL, 0, curPoseID_c, 5.0f);
				}else if((HP<=0)&&(curPoseID_c != dieID_c)){
					curPoseID_c = dieID_c;
					actor_c.SetCurrentAction(NULL, 0, curPoseID_c, 5.0f);
				}
			}
	}

	//�����R���ˮ`�P�w
	void attackHit(int number){
		float angleLimit;
		float lengthLimit;
		int damage;
		float angleLocal;
		float lengthLocal;

		//�ۦ��ˮ`�d��]�w�A�i�ھ�enemy_category�]�w
		if(number==1){
			if(enemy_category==0){
				angleLimit=60.0f;
				lengthLimit=135.0f;
				damage=1;
			}else if(enemy_category==1){
				angleLimit=60.0f;
				lengthLimit=135.0f;
				damage=2;
			}else if(enemy_category==2){
				angleLimit=60.0f;
				lengthLimit=135.0f;
				damage=8;
			}
		}

		//Donzo�H�~��enemy�u�|��player�MDonzo�������P�w
		if(index!=0){

			angleLocal=getAngleWithCharacterID(actorID_c,playerID_c,true);
			lengthLocal=GetDistanceWithCharacterID(actorID_c,playerID_c);

			if((angleLocal<=angleLimit)&&(lengthLocal<=lengthLimit)){
				damageToPlayer=damage;
			}

			angleLocal=getAngleWithCharacterID(actorID_c,donzoID_c,true);
			lengthLocal=GetDistanceWithCharacterID(actorID_c,donzoID_c);

			if((angleLocal<=angleLimit)&&(lengthLocal<=lengthLimit)){
				damageToEnemies[0]=damage;
			}
		}else{
			for(int y=1;y<enemySize;y++){
				angleLocal=getAngleWithCharacterID(actorID_c,enemiesID[y],true);
				lengthLocal=GetDistanceWithCharacterID(actorID_c,enemiesID[y]);
				
				if((angleLocal<=angleLimit)&&(lengthLocal<=lengthLimit)){
						damageToEnemies[y]=damage;
				}
			}
		}
		//Donzo�h�O��Ҧ���Lenemy�������P�w
	}

	//����target��V�]�w
	void turnSetting(){
		if(curPoseID_c!=dieID_c){
			float angle;
			
			angle=getAngleWithCharacterID(actorID_c,targetID_c,false);

			if(angle>25.0f){
				
					getResultFdir(targetID_c,actorID_c,savedTurnTarget);
					
					actor_c.TurnRight(5.0f);

					if(getAngleWithCharacterID(actorID_c,targetID_c,false)<angle){
						turnRLflag=0;
					}else{
						turnRLflag=1;
					}

					actor_c.TurnRight(-5.0f);

					if(curPoseID_c==idleID_c){
						curPoseID_c = runID_c;
						actor_c.SetCurrentAction(NULL, 0, curPoseID_c, 5.0f);
					}
			}
		}
	}

	void findRoute(){
		bool continueFlag=true;

		//�p�G�O�e�@���o�ͼ��H����ƥ�h����A������կ�_�e�i�A�Y�i�h�i�J�e�i���q
		if(blockTurning){
			if(curPoseID_c!=runID_c){
					curPoseID_c=runID_c;
					actor_c.SetCurrentAction(NULL, 0, curPoseID_c, 5.0f);
			}

			actor_c.TurnRight(turnSpeed);

			continueFlag=testIFforward(actorID_c,playerID_c,50.0f);
			if(continueFlag){
				for(int y=0;y<enemySize;y++){
					if(y!=index){
						continueFlag=testIFforward(actorID_c,enemiesID[y],50.0f);
						if(!continueFlag){
							break;
						}
					}
				}
			}

			if(continueFlag){
				blockTurning=false;
			}
		}else{
			if(curPoseID_c!=runID_c){
					curPoseID_c=runID_c;
					actor_c.SetCurrentAction(NULL, 0, curPoseID_c, 5.0f);
			}

			actor_c.MoveForward(walkSpeed, TRUE, FALSE, FALSE, FALSE);
			blockCounter--;

			continueFlag=testIFforward(actorID_c,playerID_c,50.0f);
			if(continueFlag){
				for(int y=0;y<enemySize;y++){
					if(y!=index){
						continueFlag=testIFforward(actorID_c,enemiesID[y],50.0f);
						if(!continueFlag){
							break;
						}
					}
				}
			}

			if(!continueFlag){
				blockCounter=20;
				blockTurning=true;
			}
		}
		//�M�����e�i���q�A�u���o�Ӫ��A�U�~�|���blockCounter�A�Y�L�{�A�׵o�͸I���h���s�i�J���ඥ�q�í��mblockCounter
	}

	void runAndAttack(){
		if(turnRLflag==-1){
			bool continueFlag=true;
			//�P�_�O�_����H����

			continueFlag=testIFforward(actorID_c,playerID_c,50.0f);
			if(continueFlag){
				for(int y=0;y<enemySize;y++){
					if(y!=index){
						continueFlag=testIFforward(actorID_c,enemiesID[y],50.0f);
						if(!continueFlag){
							break;
						}
					}
				}
			}

			if(continueFlag){

				//�j��@�wrange�N�]�Vtarget
				if(GetDistanceWithCharacterID(actorID_c,targetID_c)>toTargetRange){
					if(curPoseID_c!=runID_c){
						curPoseID_c=runID_c;
						actor_c.SetCurrentAction(NULL, 0, curPoseID_c, 5.0f);
					}
					actor_c.MoveForward(walkSpeed, TRUE, FALSE, FALSE, FALSE);
				}else{
					attack();
				}
				//�Ϥ�����
			}else{
				blockCounter=20;
				blockTurning=true;
				//�Y�L�k�e�i�h�i�J�M�������ඥ�q
			}
		}
	}

	void attack(){
		//timeCounter�bdoActions���Q�˼ƨ�-1�h�i�A�׶i�����
		if(timeCounter<0){
				curPoseID_c = attack1ID_c;
				actor_c.SetCurrentAction(NULL, 0, curPoseID_c, 5.0f);
				timeCounter=hitCounter+15;
				//timeCounter�O�ܤU�@���������`�ɶ��AhitCounter�O�ھ�enemy_category�]�w���Τ@�����P�w�ɶ��A�۷��w��
				//+15��15�N�O���P�����i�H�ۭq���o�ۨ�R�����ɶ��t
				//�n�[�J���P�ۦ��������ӥi�H��random�M�w���P��attackID�A��M�ɶ��t�N�i�H�ھڤ��PattackID�ӳ]�w�F
		}else{
				if(curPoseID_c!=idleID_c){
						curPoseID_c = idleID_c;
						actor_c.SetCurrentAction(NULL, 0, curPoseID_c, 5.0f);
				}
		}
		//�_�h���m
	}
	
	//��Vtarget
	void turn(){
		float actrFDir[3];
		float actrUDir[3];
		
		if(turnRLflag!=-1){

			if(curPoseID_c!=runID_c){
					curPoseID_c=runID_c;
					actor_c.SetCurrentAction(NULL, 0, curPoseID_c, 5.0f);
			}

			//����
			turnHelper();

			//�৹��Y�����p�����t�״N�����]�w�A�è���turnRLflag
			actor_c.GetDirection(actrFDir,actrUDir);
		
			if(getAngle(savedTurnTarget,actrFDir,false)<=turnSpeed){
				savedTurnTarget[2]=actrFDir[2];
				actor_c.SetDirection(savedTurnTarget,actrUDir);
				turnRLflag=-1;

				if(index==0){
					curPoseID_c = idleID_c;
					actor_c.SetCurrentAction(NULL, 0, curPoseID_c, 5.0f);
				}
			}
		}
	}
	
	//�̾�turnRLflag����Υk��
	void turnHelper(){
		if(turnRLflag==0){
			actor_c.TurnRight(turnSpeed);
		}else if(turnRLflag==1){
			actor_c.TurnRight(-turnSpeed);
		}
	}
};

enemy * enemyArray[enemySize];

CHARACTERid enemyID[enemySize];

class Player{
public:
	Player(Controller*controller_input,float*pos_c,float*fDir_c,float*uDir_c,float turnSpeed_input,int walkSpeed_input,int HP_input){
		//��l�Ƹ��
		
		actorID_c = scene.LoadCharacter("Lyubu2");
   
        actor_c.ID(actorID_c);
		actor_c.SetDirection(fDir_c, uDir_c);
		actor_c.SetTerrainRoom(terrainRoomID, 10.0f);
        beOK = actor_c.PutOnTerrain(pos_c);
		
		idleID_c = actor_c.GetBodyAction(NULL, "Idle");
        runID_c = actor_c.GetBodyAction(NULL, "Run");
		dieID_c = actor_c.GetBodyAction(NULL, "Die");
		hurtID_c=actor_c.GetBodyAction(NULL, "LeftDamaged");
		atk1ID_c=actor_c.GetBodyAction(NULL, "NormalAttack1");
		atk2ID_c=actor_c.GetBodyAction(NULL, "NormalAttack3");
		atk3ID_c=actor_c.GetBodyAction(NULL, "NormalAttack2");
		atk4ID_c=actor_c.GetBodyAction(NULL, "NormalAttack4");
		Hatk1ID_c=actor_c.GetBodyAction(NULL, "HeavyAttack1");
		Hatk2ID_c=actor_c.GetBodyAction(NULL, "HeavyAttack2");
		Hatk3ID_c=actor_c.GetBodyAction(NULL, "HeavyAttack3");
		UatkID_c=actor_c.GetBodyAction(NULL, "UltimateAttack");
		guardID_c=actor_c.GetBodyAction(NULL, "guard");
   
		curPoseID_c = idleID_c;
		actor_c.SetCurrentAction(NULL, 0, curPoseID_c);
		actor_c.Play(START, 0.0f, FALSE, TRUE);
		actor_c.TurnRight(90.0f);

		
		controller_c=controller_input;
		walkFlag=DO_NOTHING;
		timeCounter=-1;
		turnRLflag=-1;
		turnSpeed=turnSpeed_input;
		walkSpeed=walkSpeed_input;
		HP=HP_input;

		for(int y=0;y<enemySize;y++){
			damageToEnemies[y]=0;
		}
	}

	void setTurnSpeed(float input){
		turnSpeed=input;
	}

	float getTurnSpeed(){
		return turnSpeed;
	}

	FnCharacter*getActor(){
		return &actor_c;
	}

	int getHP(){
		return HP;
	}

	CHARACTERid getID(){
		return actorID_c;
	}

	//�ˬd�O�_�R���ĤH
	int ifHitEnemy(int index){
		int temp=damageToEnemies[index];
		damageToEnemies[index]=0;
		return temp;
	}

	void setEnemiesID(CHARACTERid*enemiesID_input){
		enemiesID=enemiesID_input;
	}

	//�]�wplayer�]�B�ʧ@
	void setRunningAction(bool value){
		if(value){
			if (curPoseID_c == idleID_c){
				curPoseID_c = runID_c;
				actor_c.SetCurrentAction(NULL, 0, curPoseID_c, 5.0f);
			}
		}else{
			
			if (curPoseID_c == runID_c){
				runToIdleHelper();
			}
		}
	}

	//�]�wplayer�����ʧ@
	void setAttackingAction(int index){
		
			if ((!ifAttacking())&&(curPoseID_c != hurtID_c)&&(curPoseID_c != dieID_c)){
				if(index==0){
					curPoseID_c = atk1ID_c;
					timeCounter=0;
				}else if(index==1){
					curPoseID_c = atk2ID_c;
					timeCounter=25;
				}else if(index==2){
					curPoseID_c = atk3ID_c;
					timeCounter=5;
				}
				actor_c.SetCurrentAction(NULL, 0, curPoseID_c, 5.0f);
			}
	}

	//��V�]�w
	void turnSetting(float* fDir,float turnLimitValue,float turnTsetValue){
		float playerFDir[3];
		float playerUDir[3];
		float angle;

		if(curPoseID_c==runID_c){
		
			//�O�s��e�ؼФ�V
			targetFdir[0]=fDir[0];
			targetFdir[1]=fDir[1];
			targetFdir[2]=fDir[2];

			//�⨤��M�ؼФ�V������
			actor_c.GetDirection(playerFDir,playerUDir);

			angle=getAngle(fDir,playerFDir,false);

			//���׮t�j��֭ȴN�|�i����V�]�w�ô��ե����٬O�k��|�Y�p�����A�H���]�wturnRLflag
			//�ѩ�H���Mcamera��V�����פ��঳����t�ȡA�ҥH�֭�(camera�|��n�ǹL��)����<camera������V�t�סA��v������L��ʮ�
			//Player�N�৹����W�A�S�٬O�o���o��limit�s�b�A�_�h�C������v�����ת�float�~�t�|�OturnRLflag���򬰯u
			//�y��Player����e�i
			if(angle>turnLimitValue){
				actor_c.TurnRight(turnTsetValue);

				actor_c.GetDirection(playerFDir,NULL);

				if(getAngle(fDir,playerFDir,false)<angle){
					turnRLflag=0;
				}else{
					turnRLflag=1;
				}

				actor_c.TurnRight(-turnTsetValue);
			}

		}
		//run action�ɤ~�����Ӧ�camera����V�]�w
	}


	//���ʧ@�A��GameAI�I�s
	void doActions(int skip,CHARACTERid firstAttackerID,int totalDamage){
		
		beHit(firstAttackerID,totalDamage);

		if((curPoseID_c==runID_c)||(curPoseID_c==idleID_c)){
			
			actor_c.Play(LOOP, (float) skip, FALSE, TRUE);
		
			if(curPoseID_c==runID_c){
				turn();
				run();
			}
		}else{
			BOOL4 playOver=actor_c.Play(ONCE, (float) skip, FALSE, TRUE);

			//������ʧ@�}�l�h�[�H�M�w�O�_�i�������P�w
			if(timeCounter!=-1){
				if(timeCounter==0){
					if(curPoseID_c==atk1ID_c){
						attackHit(1);
					}else if(curPoseID_c==atk2ID_c){
						attackHit(2);
					}else if(curPoseID_c==atk3ID_c){
						attackHit(3);
					}
				}
				timeCounter--;
			}

			//����ONCE�ʧ@�N�ݲ����䦳�_�Q���ۨM�widle or run
			if ((playOver == FALSE)&&(curPoseID_c != dieID_c)){
				if(controller_c->getMoveDirectionFlag()!=-1){
					curPoseID_c = runID_c;
				}else{
					curPoseID_c = idleID_c;
				}
				actor_c.SetCurrentAction(NULL, 0, curPoseID_c,5.0f);
			}
		}
	}

	//�M���e�i���\flag
	void setWalkFlag(){
		walkFlag=DO_NOTHING;
	}

	//get�e�i���\flag
	int getWalkFlag(){
		return walkFlag;
	}

	//�O�_���b����
	bool ifAttacking(){
		if((curPoseID_c == atk1ID_c)||(curPoseID_c == atk2ID_c)||(curPoseID_c == atk3ID_c)){
			return true;
		}else{
			return false;
		}
	}

private:
	ACTIONid idleID_c,curPoseID_c,dieID_c,hurtID_c,runID_c,atk1ID_c,atk2ID_c,atk3ID_c,atk4ID_c,Hatk1ID_c,Hatk2ID_c,Hatk3ID_c,UatkID_c,guardID_c;
	CHARACTERid actorID_c;
	
	CHARACTERid*enemiesID;
	int damageToEnemies[enemySize];
	float targetFdir[3];
	float turnSpeed;
	int turnRLflag;
	int timeCounter;
	FnCharacter actor_c;
	Controller*controller_c;
	int walkFlag;//���L���\�e�i
	float walkSpeed;
	int HP;

	//�P�_�O�_�Q�����ó]�w�����ʧ@
	void beHit(CHARACTERid firstAttackerID,int totalDamage){

			float resultFdir[3];
			float fdir[3],udir[3];
		
			if(totalDamage>0){	

				//�Q�����N���m�����ʧ@counter
				timeCounter=-1;	
				
				if(HP>0){
					HP-=totalDamage;
					getResultFdir(firstAttackerID,actorID_c,resultFdir);
					actor_c.GetDirection(fdir,udir);
					resultFdir[2]=fdir[2];
					actor_c.SetDirection(resultFdir,udir);
				}

				if(HP>0){
					curPoseID_c = hurtID_c;
					actor_c.SetCurrentAction(NULL, 0, curPoseID_c, 5.0f);
				}else if((HP<=0)&&(curPoseID_c != dieID_c)){
					curPoseID_c = dieID_c;
					actor_c.SetCurrentAction(NULL, 0, curPoseID_c, 5.0f);
				}
			}
	}

	//�����R���ˮ`�P�w
	void attackHit(int number){
		float angleLocal;
		float lengthLocal;
		float angleLimit;
		float lengthLimit;
		int damage;

		if(number==1){
			lengthLimit=135.0f;
			angleLimit=30.0f;
			damage=1;
		}else if(number==2){
			lengthLimit=135.0f;
			angleLimit=60.0f;
			damage=2;
		}else if(number==3){
			lengthLimit=135.0f;
			angleLimit=180.0f;
			damage=5;
		}

		//�Y���ĤH�b�����d�򤺴N�O����L���ˮ`
		for(int y=1;y<enemySize;y++){
			angleLocal=getAngleWithCharacterID(actorID_c,enemiesID[y],true);
			lengthLocal=GetDistanceWithCharacterID(actorID_c,enemiesID[y]);
				
			if((angleLocal<=angleLimit)&&(lengthLocal<=lengthLimit)){
					damageToEnemies[y]=damage;
			}
		}
	}
	
	void turn(){
		float playerFDir[3];
		float playerUDir[3];
		
		if(turnRLflag!=-1){
			//����
			turnHelper();

			//�৹��Y�����p�����t�״N�����]�w�A�è���turnRLflag
			actor_c.GetDirection(playerFDir,playerUDir);
		
			if(getAngle(targetFdir,playerFDir,false)<=turnSpeed){
				targetFdir[2]=playerFDir[2];
				actor_c.SetDirection(targetFdir,playerUDir);
				turnRLflag=-1;
				
				runToIdleHelper();
			}
		}
	}
	
	//�̾�turnRLflag����Υk��
	void turnHelper(){
		if(turnRLflag==0){
			actor_c.TurnRight(turnSpeed);
		}else if(turnRLflag==1){
			actor_c.TurnRight(-turnSpeed);
		}
	}

	//��S���]�B����V�ɤ~�]idle Action
	void runToIdleHelper(){
		if (!FyCheckHotKeyStatus(FY_W)&&!FyCheckHotKeyStatus(FY_A)&&!FyCheckHotKeyStatus(FY_S)&&!FyCheckHotKeyStatus(FY_D)&&(turnRLflag==-1)&&(curPoseID_c!=dieID_c)) {
				curPoseID_c = idleID_c;
				actor_c.SetCurrentAction(NULL, 0, curPoseID_c, 5.0f);
		}
	}

	//�e�i
	void run(){
		if((controller_c->getMoveDirectionFlag()!=-1)&&(turnRLflag==-1)){
			
			bool continueFlag=true;
			//�P�_�O�_����ĤH
			 for(int y=0;y<enemySize;y++){
				continueFlag=testIFforward(actorID_c,enemiesID[y],50.0f);
				if(!continueFlag){
					break;
				}
			 }

			if(continueFlag){
				walkFlag=actor_c.MoveForward(walkSpeed, TRUE, FALSE, FALSE, FALSE);
			}
		}
	}
};

class Camera{
	public:
		Camera(Player*player_input,Controller*controller_input,float radius,float height,float cameraSpeedH_input,float cameraSpeedV_input,float cameraSpeedD_input){
			float fDir[3],uDir[3];
			
			//��v����l��
			cID = scene.CreateObject(CAMERA);
			
			camera_c.ID(cID);
			camera_c.SetNearPlane(5.0f);
			camera_c.SetFarPlane(100000.0f);
  
			cpID_c = scene.CreateObject(OBJECT);
			cp_c.ID(cpID_c);
			camera_c.SetParent(cpID_c);

			cp_c.GetDirection(fDir, uDir);
			camera_c.SetDirection(fDir, uDir);

			//Camera��Ʀ�����l��
			player_c=player_input;
			controller_c=controller_input;

			//camera turning speed for vertical or horizontal
			cameraSpeedH=cameraSpeedH_input;
			cameraSpeedV=cameraSpeedV_input;
			cameraSpeedD=cameraSpeedD_input;

			turnLimitValue=0.5*cameraSpeedH;
			turnTestValue=0.5*turnLimitValue;
			
			constHeight_c=height;
			constRadius_c=radius; 
			constSide_c=sqrt(pow(radius,2)+pow(height,2));
			savedProportion_c=height/radius;
			
			//camera�ɭ�limit
			upLimit=0.9*constSide_c;

			pushCamera(radius,height);

			//debuger
			bug4=constRadius_c;
			bug5=constHeight_c;
			bug6=constSide_c;

			controlDistanceFlag=false;

		}

		//GameAI call this
		void doActions(){	

			//�ˬd��v���O�_�o��(�æ�)�]float overflow�ɭP��180�צ첾�A���Ȧs�B�z�edata
			float localTestAngle;
			float testSavedFdir[3],testSavedUdir[3],testSavedPos[3];
			float testSavedFdirAfter[3],testSavedUdirAfter[3],testSavedPosAfter[3];
			float height_l=height_c;
			float radius_l=radius_c; 
			float side_l=side_c;
			float savedProportion_l=savedProportion_c;
			float targetCameraRadius_l=targetCameraRadius;
			bool controlDistanceFlag_l=controlDistanceFlag;

			cp_c.GetDirection(testSavedFdir,testSavedUdir);
			cp_c.GetPosition(testSavedPos);

			//camera turn vertical or horizontal or both
			turn();
			//camera move with player and test minus distance
			moveAndTest();
			//minus distance to player if needed
			minusDistanceToPlayer();

			//debuger
			bug=radius_c;
			bug2=height_c;
			bug3=side_c;

			cp_c.GetDirection(testSavedFdirAfter,testSavedUdirAfter);
			cp_c.GetPosition(testSavedPosAfter);

			localTestAngle=getAngle(testSavedFdir,testSavedFdirAfter,false);
			if(localTestAngle>=90.0f){
				cp_c.SetDirection(testSavedFdir,testSavedUdir);
				cp_c.SetPosition(testSavedPos);
				height_c=height_l;
				radius_c=radius_l; 
				side_c=side_l;
				savedProportion_c=savedProportion_l;
				targetCameraRadius=targetCameraRadius_l;
				controlDistanceFlag=controlDistanceFlag_l;
				testAngle=localTestAngle;
			}
			//float overflow�ҥ~�B�z�A�����˱�camera�o�@frame���ܰʡA�ڲq�U�@frame�|�]Player�����ʨϨҥ~���|�s��o��
			//�Y�]���ɭP�{�����Y�����~�i��n��o��test�t�C��code�R��
			//update:�g���ըҥ~�B�z���`
		}
		

		void setCameraSpeedH(float input){
			cameraSpeedH=input;
		}

		void setCameraSpeedV(float input){
			cameraSpeedV=input;
		}

		float getCameraSpeedH(){
			return cameraSpeedH;
		}

		float getCameraSpeedV(){
			return cameraSpeedV;
		}

		//�̾�camera�{�b��V�P�_player����V����
		void getTurningTargetDir(){
			float fDir[3];
			float uDir[3];
			int localflag;

			OBJECTid objID = scene.CreateObject(OBJECT);
			FnObject obj;
			obj.ID(objID);
			//��obj�N��camera�H�K�]float�~�t�L�k�Ocamera�����k��A�ӨϼƬ��camera�}�l����
			
			localflag=controller_c->getMoveDirectionFlag();
			
			//�����۲�����~����v����V
			if(localflag!=-1){
			
				//�p��H�����l�ܪ���v����V
				cp_c.GetDirection(fDir,uDir);
				fDir[2]=0.0f;
				obj.SetDirection(fDir,uDir);

				if(localflag==1){
					obj.TurnRight(180.0f);
				}else if(localflag==2){
					obj.TurnRight(-90.0f);
				}else if(localflag==3){
					obj.TurnRight(90.0f);
				}else if(localflag==4){
					obj.TurnRight(-45.0f);
				}else if(localflag==5){
					obj.TurnRight(-135.0f);
				}else if(localflag==6){
					obj.TurnRight(45.0f);
				}else if(localflag==7){
					obj.TurnRight(135.0f);
				}

				obj.GetDirection(fDir,NULL);
				
				//�ᵹplayer�]�w��V
				player_c->turnSetting(fDir,turnLimitValue,turnTestValue);

			}
		}
			
	private:
		OBJECTid cpID_c;
		FnCamera camera_c;
		FnObject cp_c;
		float constHeight_c;
		float constRadius_c; 
		float constSide_c;
		float height_c;
		float radius_c; 
		float side_c;
		float savedProportion_c;
		float targetCameraRadius;
		bool controlDistanceFlag;
		Player*player_c;
		Controller*controller_c;
		float cameraSpeedH;
		float cameraSpeedV;
		float cameraSpeedD;
		float upLimit;
		float turnLimitValue;
		float turnTestValue;

		//camera������V
		void turnHelperH(){
			FnCharacter* actrLocal=player_c->getActor();

			if(controller_c->getCameraDirectionFlagH()==0){
				actrLocal->TurnRight(-cameraSpeedH);
				pushCamera(radius_c,height_c);
				actrLocal->TurnRight(cameraSpeedH);
			}else if(controller_c->getCameraDirectionFlagH()==1){
				actrLocal->TurnRight(cameraSpeedH);
				pushCamera(radius_c,height_c);
				actrLocal->TurnRight(-cameraSpeedH);
			}
		
		}

		//camera�����ɭ�
		void turnHelperV(){
			float hypotenuse;
			float localHeight;

			//���e�����
			hypotenuse=sqrt(pow(radius_c,2)+pow(height_c,2));

			//��ڤɭ�camera,�æs����M������Ȩ��Y��������,���ܦhlimit params
			if(controller_c->getCameraDirectionFlagV()==0){
				if(height_c<upLimit){
					localHeight=height_c;
					height_c+=cameraSpeedV;
					if(height_c>=hypotenuse){
						height_c=localHeight;
						controller_c->upArrowOff();
						controller_c->setCameraDirectionFlagV();
					}else{
						if(height_c>=upLimit){
							height_c=upLimit;
						}
						radius_c=sqrt(pow(hypotenuse,2)-pow(height_c,2));
						savedProportion_c=height_c/radius_c;
						pushCamera(radius_c,height_c);
					}
				}else{
					controller_c->upArrowOff();
					controller_c->setCameraDirectionFlagV();
				}
			}else if(controller_c->getCameraDirectionFlagV()==1){
				localHeight=height_c;
				height_c-=cameraSpeedV;
				if(abs(height_c)>=hypotenuse){
					height_c=localHeight;
					controller_c->downArrowOff();
					controller_c->setCameraDirectionFlagV();
				}else{
					radius_c=sqrt(pow(hypotenuse,2)-pow(height_c,2));
					savedProportion_c=height_c/radius_c;
					pushCamera(radius_c,height_c);
				}
			}
		
		}

		//let camera go with player and test minus distance to player
		void moveAndTest(){
				FnCharacter* actrLocal=player_c->getActor();
				float actrFdir[3],actrUdir[3];
				float craFdir[3];
				float innerFlag=false;
				float localSide;
				float localRaduis;
				float localHeight;

				if((controller_c->getCameraDirectionFlagH()!=-1)||(controller_c->getCameraDirectionFlagV()!=-1)||(player_c->getWalkFlag()==WALK)){

					actrLocal->GetDirection(actrFdir,actrUdir);
					cp_c.GetDirection(craFdir,NULL);
					craFdir[2]=actrFdir[2];
					actrLocal->SetDirection(craFdir,actrUdir);

					float savedRadius=radius_c;
					float savedHeight=height_c;
					
					//�O�H�����ʦ�camera no turning��camera�|��۲���
					pushCamera(radius_c,height_c);

					//�Ycamera�o�͸I���h������x�s����ȴ�ֱ�������쥼�I���A���o�u�O�p��ؼ��Y��ȥ�����Y��
					if(testHit()<=0){
						while(testHit()<=0){

							radius_c-=1.0f;					
							height_c=radius_c*savedProportion_c;
							
							pushCamera(radius_c,height_c);
						}
						targetCameraRadius=radius_c;
						controlDistanceFlag=true;
					}else{
						while((testHit()>0)&&(!innerFlag)){
							localRaduis=radius_c;
							localHeight=height_c;
							radius_c+=1.0f;
							height_c=radius_c*savedProportion_c;
							localSide=sqrt(pow(radius_c,2)+pow(height_c,2));
							
							if(localSide>=constSide_c){
								radius_c=localRaduis;
								height_c=localHeight;
								innerFlag=true;
							}
							pushCamera(radius_c,height_c);
						}
						targetCameraRadius=radius_c;
						controlDistanceFlag=true;
					}
					//�Ϥ��Y���o�͸I���h�n����W�[camera���������A�׵o�͸I���ε����l���������

					pushCamera(savedRadius,savedHeight);
					//������Y��ؼЪ�camera�k��

					actrLocal->SetDirection(actrFdir,actrUdir);

					//init WalkFlag �o�ˤ~���|player�S�����Bcamera�S�b����]�@��push camera
					player_c->setWalkFlag();
				
				}
		}

		//�w���v���Y����v���bPlayer�V�U���ʩM���k���v���P�ɵo�ͮɦ����o����v�������ܤϤ�V�����D�A�i��
		//��player��ָ�camera���Z�����Y��X������?�ݽT�{��]�í״_

		//UPDATE:��������Player��V���k��N���ӵo�ͤF�A�ܤּƦ��sĶ�������F5,60���u���@���_�o�A���Ӻ�U��
		//�ܥi�H�L�������šA���~�bdoActions���s�W���C���v�ҥ~�o�ͮɪ��״_
		void minusDistanceToPlayer(){
			//����Y��camera�Z��

			FnCharacter* actrLocal=player_c->getActor();
			float actrFdir[3],actrUdir[3];
			float craFdir[3];

			if(controlDistanceFlag){

					actrLocal->GetDirection(actrFdir,actrUdir);
					cp_c.GetDirection(craFdir,NULL);
					craFdir[2]=actrFdir[2];
					actrLocal->SetDirection(craFdir,actrUdir);

					if(targetCameraRadius>radius_c){
						radius_c+=cameraSpeedD;
						if(radius_c>=targetCameraRadius){
							radius_c=targetCameraRadius;
							controlDistanceFlag=false;
						}
					}else if(targetCameraRadius<radius_c){
						radius_c-=cameraSpeedD;
						if(radius_c<=targetCameraRadius){
							radius_c=targetCameraRadius;
							controlDistanceFlag=false;
						}
					}else if(targetCameraRadius==radius_c){
						controlDistanceFlag=false;
					}
					height_c=radius_c*savedProportion_c;

					pushCamera(radius_c,height_c);

					actrLocal->SetDirection(actrFdir,actrUdir);
				}
		}

		//camera�I������
		int testHit()
		{
			float dirt[3], origint[3];  
			dirt[0] = 0.0f;
			dirt[1] = 0.0f;
			dirt[2] = -1.0f;
			cp_c.GetPosition(origint);

			return(terrain.HitTest(origint, dirt));
		}
		
		
		void pushCamera(float radius,float height){
			if(radius>0){
				//����v���æs�����,��,�����
				float fDir[3],uDir[3],actPos[3],cpPos[3];
				FnCharacter *actrLocal;
				actrLocal=player_c->getActor();

				actrLocal->GetPosition(actPos);
				actPos[2] += 50.0f;
				actrLocal->GetDirection(fDir, uDir);
				cp_c.SetPosition(actPos);
				cp_c.SetDirection(fDir, uDir);
				cp_c.MoveForward(-radius);
				cp_c.MoveUp(height);
				cp_c.GetPosition(cpPos);
				for (int i = 0; i < 3; i++){
					fDir[i] = actPos[i] - cpPos[i];
				}
				cp_c.SetDirection(fDir, NULL);

				radius_c=radius;
				height_c=height;
				side_c=sqrt(pow(radius,2)+pow(height,2));
			}else{
				controlDistanceFlag=false;
			}
		}

		void turn(){

			if((controller_c->getCameraDirectionFlagH()!=-1)||(controller_c->getCameraDirectionFlagV()!=-1)){
	
				FnCharacter* actrLocal=player_c->getActor();
				float actrFdir[3],actrUdir[3];
				float craFdir[3];

				//���N�H���]�����e��v���P�V,�`�N�H��Z�b���n���
				actrLocal->GetDirection(actrFdir,actrUdir);
				cp_c.GetDirection(craFdir,NULL);
				craFdir[2]=actrFdir[2];
				actrLocal->SetDirection(craFdir,actrUdir);
				
				//�ɭ�camera
				if(controller_c->getCameraDirectionFlagV()!=-1){
					turnHelperV();
				}
				
				//��H���A����v��
				if(controller_c->getCameraDirectionFlagH()!=-1){
					turnHelperH();		
				}

				//player�¦V�_��
				actrLocal->SetDirection(actrFdir,actrUdir);
			}
		}
		
};

Controller*controller;
Camera*camera;
Player *player;

// some globals
int frame = 0;
int oldX, oldY, oldXM, oldYM, oldXMM, oldYMM;

// hotkey callbacks
void QuitGame(BYTE, BOOL4);
void Movement(BYTE, BOOL4);

// timer callbacks
void GameAI(int);
void RenderIt(int);

// mouse callbacks
void InitPivot(int, int);
void PivotCam(int, int);
void InitMove(int, int);
void MoveCam(int, int);
void InitZoom(int, int);
void ZoomCam(int, int);

/*------------------
  the main program
  C.Wang 1010, 2014
 -------------------*/
void FyMain(int argc, char **argv)
{
	
	// create a new world
   beOK = FyStartFlyWin32("NTU@2014 Homework #01 - Use Fly2", 0, 0, 800, 600, FALSE);

   // setup the data searching paths
   FySetShaderPath("Data\\NTU5\\Shaders");
   FySetModelPath("Data\\NTU5\\Scenes");
   FySetTexturePath("Data\\NTU5\\Scenes\\Textures");
   FySetScenePath("Data\\NTU5\\Scenes");

   // create a viewport
   vID = FyCreateViewport(0, 0, 800, 600);
   FnViewport vp;
   vp.ID(vID);

   // create a 3D scene
   sID = FyCreateScene(10);
   
   scene.ID(sID);

   // load the scene
   scene.Load("gameScene01");
   scene.SetAmbientLights(1.0f, 1.0f, 1.0f, 0.6f, 0.6f, 0.6f);

   // load the terrain
   tID = scene.CreateObject(OBJECT);
   terrain.ID(tID);
   BOOL beOK1 = terrain.Load("terrain");
   terrain.Show(FALSE);

   // set terrain environment
   terrainRoomID = scene.CreateRoom(SIMPLE_ROOM, 10);
   FnRoom room;
   room.ID(terrainRoomID);
   room.AddObject(tID);

   // load the character
   FySetModelPath("Data\\NTU5\\Characters");
   FySetTexturePath("Data\\NTU5\\Characters");
   FySetCharacterPath("Data\\NTU5\\Characters");

   // put the character on terrain
   float pos[3], fDir[3], uDir[3];
   pos[0] = 3569.0f; pos[1] = -3208.0f; pos[2] = 1000.0f;
   fDir[0] = 1.0f; fDir[1] = 1.0f; fDir[2] = 0.0f;
   uDir[0] = 0.0f; uDir[1] = 0.0f; uDir[2] = 1.0f;

   //init managers
   controller=new Controller();
   player=new Player(controller,pos,fDir,uDir,15.0f,10.0f,100);
   camera=new Camera(player,controller,700.0f,50.0f,2.5f,10.0f,40.0f);

   //enemySize define�b�̫e���A�ODonzo+�p�L�ƶq+�ĤH�f���A0���Ndonzo�AenemySize-1���ĤH�f���A
   //�ĤH�f���o�̴Nnew�X�ӧ����P�w�B��|�����K�A�ҥH�u�n����L��pos���]��-99999.0f(�Ψ�L�ݤ������a��A�p�Фl��)�A
   //�n�ήɦAset�ܱ��X�{���a��N�n�F
   //�p�L���F�@�q�ɶ���n�������Ӥ]�O�o��~�o�ˤ~���Τ@��new�Bdelete�B�ι�access enemyArray������
   fDir[0] = -1.0f; fDir[1] = -1.0f; fDir[2] = -0.0f;

   pos[0]-=150.0f;
   enemyArray[0]=new enemy(-1,player->getID(),"Donzo2",pos,fDir,uDir,15.0f,5.0f,135.0f,50,0);
   enemyID[0]=enemyArray[0]->getID();

   for(int y=1;y<enemySize-1;y++){
	   pos[0]-=150.0f;
	   pos[1]+=150.0f;
	   enemyArray[y]=new enemy(enemyArray[0]->getID(),player->getID(),"Robber02",pos,fDir,uDir,15.0f,7.5f,75.0f,10,y);
	   enemyID[y]=enemyArray[y]->getID();
   }

   pos[0]=-99999.0f;
   pos[1]=-99999.0f;
   pos[2]=-99999.0f;
   enemyArray[enemySize-1]=new enemy(enemyArray[0]->getID(),player->getID(),"Lyubu2",pos,fDir,uDir,15.0f,5.0f,75.0f,50,enemySize-1);
   enemyID[enemySize-1]=enemyArray[enemySize-1]->getID();

   //init enemyID array for instance
   player->setEnemiesID(enemyID);
   
   for(int y=0;y<enemySize;y++){
		enemyArray[y]->setEnemiesID(enemyID);
   }

   // setup a point light
   FnLight lgt;
   lgt.ID(scene.CreateObject(LIGHT));
   lgt.Translate(70.0f, -70.0f, 70.0f, REPLACE);
   lgt.SetColor(1.0f, 1.0f, 1.0f);
   lgt.SetIntensity(1.0f);

   // create a text object for displaying messages on screen
   textID = FyCreateText("Trebuchet MS", 18, FALSE, FALSE);

   // set Hotkeys
   FyDefineHotKey(FY_ESCAPE, QuitGame, FALSE);  // escape for quiting the game
   FyDefineHotKey(FY_UP, Movement, FALSE);      // Up for moving forward
   FyDefineHotKey(FY_RIGHT, Movement, FALSE);   // Right for turning right
   FyDefineHotKey(FY_LEFT, Movement, FALSE);
   FyDefineHotKey(FY_DOWN, Movement, FALSE);// Left for turning left
   FyDefineHotKey(FY_W, Movement, FALSE);
   FyDefineHotKey(FY_A, Movement, FALSE);
   FyDefineHotKey(FY_S, Movement, FALSE);
   FyDefineHotKey(FY_D, Movement, FALSE);
   FyDefineHotKey(FY_Z, Movement, FALSE);
   FyDefineHotKey(FY_X, Movement, FALSE);
   FyDefineHotKey(FY_C, Movement, FALSE);

   // define some mouse functions
   FyBindMouseFunction(LEFT_MOUSE, InitPivot, PivotCam, NULL, NULL);
   FyBindMouseFunction(MIDDLE_MOUSE, InitZoom, ZoomCam, NULL, NULL);
   FyBindMouseFunction(RIGHT_MOUSE, InitMove, MoveCam, NULL, NULL);

   // bind timers, frame rate = 30 fps
   FyBindTimer(0, 30.0f, GameAI, TRUE);
   FyBindTimer(1, 30.0f, RenderIt, TRUE);

   // invoke the system
   FyInvokeFly(TRUE);

   //kill managers
   delete player;
   delete camera;
   delete controller;
}


/*-------------------------------------------------------------
  30fps timer callback in fixed frame rate for major game loop
  C.Wang 1103, 2007
 --------------------------------------------------------------*/
void GameAI(int skip)
{
	int totalDamage;
	CHARACTERid firstAttackerID;
	CHARACTERid targetID;
	int targetHP;
	
	//�]�w����X��
	controller->setFlags();
	//get player turning target direction by camera direction if needed
	camera->getTurningTargetDir();

	
	//�o���player��damage�M������(�u���Ĥ@��)
	totalDamage=0;
	firstAttackerID=99999;
	for(int y=0;y<enemySize;y++){
		totalDamage+=enemyArray[y]->ifHitPlayer();
		if((totalDamage>0)&&(firstAttackerID==99999)){
			firstAttackerID=enemyID[y];
		}
	}

	//player���ʧ@�ɶ��K����enemies��L�y����damage
	player->doActions(skip,firstAttackerID,totalDamage);

	camera->doActions();
	//camera�B�z�L�ۤv���ʧ@
	
	//enemies�ʧ@
	for(int y=0;y<enemySize;y++){
		//�o���target��HP
		targetID=enemyArray[y]->getTargetID();

		if(targetID==-1){
			targetHP=-1;
		}else if(targetID==player->getID()){
			targetHP=player->getHP();
		}else{
			for(int z=0;z<enemySize;z++){
				if(enemyArray[z]->getID()==targetID){
					targetHP=enemyArray[z]->getHP();
					break;
				}
			}
		}

		//�o���ۤv��damage�M������
		totalDamage=0;
		firstAttackerID=99999;

		totalDamage+=player->ifHitEnemy(y);
		if(totalDamage>0){
			firstAttackerID=player->getID();
		}

		for(int z=0;z<enemySize;z++){
			totalDamage+=enemyArray[z]->ifHitEnemy(y);
			if((totalDamage>0)&&(firstAttackerID==99999)){
				firstAttackerID=enemyID[z];
			}
		}

		//enemy���ʧ@
		enemyArray[y]->doActions(skip,targetHP,firstAttackerID,totalDamage);
	}
}


/*----------------------
  perform the rendering
  C.Wang 0720, 2006
 -----------------------*/
void RenderIt(int skip)
{
   FnViewport vp;

   // render the whole scene
   vp.ID(vID);
   vp.Render3D(cID, TRUE, TRUE);

   // get camera's data
   FnCamera camera;
   camera.ID(cID);

   float pos[3], fDir[3], uDir[3];
   camera.GetPosition(pos);
   camera.GetDirection(fDir, uDir);

   // show frame rate
   static char string[128];
   if (frame == 0) {
      FyTimerReset(0);
   }

   if (frame/10*10 == frame) {
      float curTime;

      curTime = FyTimerCheckTime(0);
      sprintf(string, "Fps: %6.2f", frame/curTime);
   }

   frame += skip;
   if (frame >= 1000) {
      frame = 0;
   }

   FnText text;
   text.ID(textID);

   text.Begin(vID);
   text.Write(string, 20, 20, 255, 0, 0);

   char d11DirS[256];
   char posS[256], fDirS[256], uDirS[256],dDirS[256],d2DirS[256],d3DirS[256],d4DirS[256],d5DirS[256],d6DirS[256];
   sprintf(posS, "pos: %8.3f %8.3f %8.3f", pos[0], pos[1], pos[2]);
   sprintf(fDirS, "facing: %8.3f %8.3f %8.3f", fDir[0], fDir[1], fDir[2]);
   sprintf(uDirS, "up: %8.3f %8.3f %8.3f", uDir[0], uDir[1], uDir[2]);
   sprintf(dDirS,  "radius_c: %8.3f ", bug);
   sprintf(d2DirS, "height_c: %8.3f ", bug2);
   sprintf(d3DirS, "side_c: %8.3f ", bug3);
   sprintf(d4DirS, "constRadius_c: %8.3f ", bug4);
   sprintf(d5DirS, "constHeight_c: %8.3f ", bug5);
   sprintf(d6DirS, "constSide_c: %8.3f ", bug6);
   sprintf(d11DirS, "testAngle: %8.3f ", testAngle);

   text.Write(posS, 20, 35, 255, 255, 0);
   text.Write(fDirS, 20, 50, 255, 255, 0);
   text.Write(uDirS, 20, 65, 255, 255, 0);
   text.Write(dDirS, 20, 80, 255, 255, 0);
   text.Write(d2DirS, 20, 95, 255, 255, 0);
   text.Write(d3DirS, 20, 110, 255, 255, 0);
   text.Write(d4DirS, 20, 125, 255, 255, 0);
   text.Write(d5DirS, 20, 140, 255, 255, 0);
   text.Write(d6DirS, 20, 155, 255, 255, 0);
   text.Write(d11DirS, 20, 170, 255, 255, 0);

   text.End();

   // swap buffer
   FySwapBuffers();
}


/*------------------
  movement control
  C.Wang 1103, 2006
 -------------------*/
void Movement(BYTE code, BOOL4 value)
{
	if (code == FY_LEFT){
		if(value){
			controller->leftArrowOn();
		}else{
			controller->leftArrowOff();
		}
	}

	if (code == FY_RIGHT){
		if(value){
			controller->rightArrowOn();
		}else{
			controller->rightArrowOff();
		}
	}

	if (code == FY_UP){
		if(value){
			controller->upArrowOn();
		}else{
			controller->upArrowOff();
		}
	}

	if (code == FY_DOWN){
		if(value){
			controller->downArrowOn();
		}else{
			controller->downArrowOff();
		}
	}
	
	if (code == FY_W){
		if(value){
			controller->WOn();
		}else{
			controller->WOff();
		}
		player->setRunningAction(value);
	}

	if (code == FY_S){
		if(value){
			controller->SOn();
		}else{
			controller->SOff();
		}
		player->setRunningAction(value);
	}

	if (code == FY_A){
		if(value){
			controller->AOn();
			controller->leftArrowOn();
		}else{
			controller->AOff();
			controller->leftArrowOff();
		}
		player->setRunningAction(value);
	}

	if (code == FY_D){
		if(value){
			controller->DOn();
			controller->rightArrowOn();
		}else{
			controller->DOff();
			controller->rightArrowOff();
		}
		player->setRunningAction(value);
	}

	if (code == FY_Z){
		if(value){
			player->setAttackingAction(0);
		}
	}

	if (code == FY_X){
		if(value){
			player->setAttackingAction(1);
		}
	}

	if (code == FY_C){
		if(value){
			player->setAttackingAction(2);
		}
	}



	
}


/*------------------
  quit the demo
  C.Wang 0327, 2005
 -------------------*/
void QuitGame(BYTE code, BOOL4 value)
{
   if (code == FY_ESCAPE) {
      if (value) {
         FyQuitFlyWin32();
      }
   }
}



/*-----------------------------------
  initialize the pivot of the camera
  C.Wang 0329, 2005
 ------------------------------------*/
void InitPivot(int x, int y)
{
   oldX = x;
   oldY = y;
   frame = 0;
}


/*------------------
  pivot the camera
  C.Wang 0329, 2005
 -------------------*/
void PivotCam(int x, int y)
{
   FnObject model;

   if (x != oldX) {
      model.ID(cID);
      model.Rotate(Z_AXIS, (float) (x - oldX), GLOBAL);
      oldX = x;
   }

   if (y != oldY) {
      model.ID(cID);
      model.Rotate(X_AXIS, (float) (y - oldY), GLOBAL);
      oldY = y;
   }
}


/*----------------------------------
  initialize the move of the camera
  C.Wang 0329, 2005
 -----------------------------------*/
void InitMove(int x, int y)
{
   oldXM = x;
   oldYM = y;
   frame = 0;
}


/*------------------
  move the camera
  C.Wang 0329, 2005
 -------------------*/
void MoveCam(int x, int y)
{
   if (x != oldXM) {
      FnObject model;

      model.ID(cID);
      model.Translate((float)(x - oldXM)*2.0f, 0.0f, 0.0f, LOCAL);
      oldXM = x;
   }
   if (y != oldYM) {
      FnObject model;

      model.ID(cID);
      model.Translate(0.0f, (float)(oldYM - y)*2.0f, 0.0f, LOCAL);
      oldYM = y;
   }
}


/*----------------------------------
  initialize the zoom of the camera
  C.Wang 0329, 2005
 -----------------------------------*/
void InitZoom(int x, int y)
{
   oldXMM = x;
   oldYMM = y;
   frame = 0;
}


/*------------------
  zoom the camera
  C.Wang 0329, 2005
 -------------------*/
void ZoomCam(int x, int y)
{
   if (x != oldXMM || y != oldYMM) {
      FnObject model;

      model.ID(cID);
      model.Translate(0.0f, 0.0f, (float)(x - oldXMM)*10.0f, LOCAL);
      oldXMM = x;
      oldYMM = y;
   }
}