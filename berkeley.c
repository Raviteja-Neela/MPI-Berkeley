//********************************************************************
//
// RAVI TEJA NEELA 
// Program : BERKELEY

//********************************************************************
//Procedure:Firstly,we need to read the complete data from the file line by line given as command line argument. Then I converted the following time values to min for making calculation or resetting the time. After converting them co-ordinator process will broadcast it’s time to all other processes so that they can calculate the differential of clock values. And in the given question it was mentioned that the clock value given in the last line of the file is for ignoring if the differential value exceeds that.so after calculating differential ignore the clock value if their value is greater the last line value. Then we need to calculate the average by ignoring the deviated clocks.Then adjust the clock value according to the average.Then reset their local clock value.
#include <string.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
int main(int argc, char *argv[])
{
	int size,rank,hh,mm,i,j,k,l,pp=0,pa,ho1,ho2,ho3,ho4,rr,uu,min,minAtOth,diffInMin,temp1,lasthh,lastmm,q,buff[100],sum=0,comToIgnore,
	count=0,temp5,temp7;
	float avg,timeAdj,myLoc,temp6;
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Status status;
	FILE * fp1;
	fp1 = fopen(argv[1],"r");
	char singleLine[100],a[10],ho[5],mo[5],result[5],finHour[5],finMin[5];
	fgets(singleLine,100,fp1);
	int s=atoi(singleLine);
	//reading lines in the file till end of the file.
	while(!feof(fp1))
	{
		fgets(singleLine,100,fp1);
		i=singleLine[0]-'0';
		j=singleLine[1]-'0';
		k=singleLine[3]-'0';
		l=singleLine[4]-'0';
		if(rank==pp)
		{
			hh=(i*10)+j;
			mm=(k*10)+l;
			
		}
		//identifying the last line in the file
		if(rank==s&&pp==size)
		{
			lasthh=(i*10)+j;
			lastmm=(k*10)+l;
			temp1=(lasthh*60)+lastmm;

		
		}
		pp++;
	}
//operations in co-ordinator process
	if(rank==s)
	{
		printf("I am process with rank %d acting as the coordinator process\n", rank); 
		


		if(hh<10){
			sprintf(ho,"0%d:",hh);
		}
		else
		{
			sprintf(ho,"%d:",hh);	
		}
		
        if(mm<10){
			sprintf(ho,"0%d",mm);
		}
		else
		{
			sprintf(mo,"%d",mm);	
		}
		

		strcat(ho, mo);
		//broadcasting from co-ordinator process
		printf( "Coordinator process is sending time to rank %d\n", rank);
		MPI_Bcast(ho,6,MPI_CHAR,s,MPI_COMM_WORLD);
				//printf( "Coordinator process is sending time to rank %d\n", rank);
				printf( "Process %d has received time %d:%d\n", rank,hh,mm);
				
			MPI_Barrier(MPI_COMM_WORLD);
		//receiving differential in min from all other sources except co-ordinator processes
		for(q=0;q<size;q++)
		{
			if(q != s)
			{   
				MPI_Recv(&diffInMin,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
				printf( "Process %d has received time differential value of %d minutes from rank %d\n", rank, diffInMin,status.MPI_SOURCE); 

				
				buff[status.MPI_SOURCE]=diffInMin;

				if(abs(buff[status.MPI_SOURCE])>temp1)
				{   
			printf( "Coordinator Process is ignoring time differential value of %d from process %d\n", buff[status.MPI_SOURCE],status.MPI_SOURCE ); 
				  
					count++;	 
				}
				else
				{
					sum=sum+buff[status.MPI_SOURCE];
				}
			}
			
		}
		//finding out average of the differential
		avg=sum/(size-count);
	    
		printf("Time differential avg is %f minutes\n", avg); 
		         ho1=ho[0]-'0';
				 ho2=ho[1]-'0';
				 ho3=ho[3]-'0';
				 ho4=ho[4]-'0';
				 rr=(ho1*10)+ho2;
				 uu=(ho3*10)+ho4;
				 
				 min=(rr*60)+uu;
//adjusting time for co-ordinator process
				 min=min+avg;
				 printf("Coordinator process has clock adjustment value of %d  minutes  to process %d\n", min, rank);
//sending time to be adjusted by processes
					for(q=0;q<size;q++)					
				    {	if(q!=s)
						{
						timeAdj=avg-buff[q];
						
                      MPI_Send(&timeAdj,1,MPI_FLOAT,q,1,MPI_COMM_WORLD);
					  printf("Coordinator process is sending the clock adjustment value of %f minutes  to process %d\n", timeAdj, q); 
						}
						
					
					}
					myLoc=(float)min/60;
				
				 temp5=(int)myLoc;
				 
				 temp6=myLoc-temp5;

				temp7=temp6*60;

				 if(temp5<10)
				 {
					sprintf(finHour,"0%d:",temp5);
		         }
		      else
		         {
					sprintf(finHour,"%d:",temp5);	
		         }
		
		if(temp7<10){
			sprintf(finMin,"0%d",temp7);
		}
		else
		{
			sprintf(finMin,"%d",temp7);	
		}
		
//the final local time at co-ordinator
		strcat(finHour, finMin);
		printf("Adjusted local time at process %d is %s\n", rank, finHour); 
	}
	
 //operation at all other processes except co-ordinator
	if(rank!=s)
	{    printf( "Coordinator process is sending time to rank %d\n", rank);
		MPI_Bcast(ho,6,MPI_CHAR,s,MPI_COMM_WORLD);
		//printf( "Coordinator process is sending time to rank %d\n", rank);
		printf( "Process %d has received time %s\n", rank,ho);
			MPI_Barrier(MPI_COMM_WORLD);
		
				 ho1=ho[0]-'0';
				 ho2=ho[1]-'0';
				 ho3=ho[3]-'0';
				 ho4=ho[4]-'0';
				 
				 rr=(ho1*10)+ho2;
				 uu=(ho3*10)+ho4;
				
				 min=(rr*60)+uu;
				 
				 minAtOth=(hh*60)+mm;
				 
				 diffInMin=minAtOth-min;

				 MPI_Send(&diffInMin,1,MPI_INT,s,0,MPI_COMM_WORLD);
				 printf( "Process %d is sending time differential value of %d minutes to process %d\n", rank, diffInMin, s);
				 MPI_Recv(&timeAdj,1,MPI_FLOAT,s,1,MPI_COMM_WORLD,&status);
				 printf("Process %d has received the clock adjustment value of %f minutes\n", rank, timeAdj); 


				 minAtOth=minAtOth+timeAdj;
				 
				 myLoc=(float)minAtOth/60;

				 temp5=(int)myLoc;
				 
				 temp6=myLoc-temp5;
				 
				temp7=temp6*60;
					
				 if(temp5<10)
				 {
					sprintf(finHour,"0%d:",temp5);
		         }
		      else
		         {
					sprintf(finHour,"%d:",temp5);	
		         }
		
		if(temp7<10){
			sprintf(finMin,"0%d",temp7);
		}
		else
		{
			sprintf(finMin,"%d",temp7);	
		}
		
//final local time at all other processes
		strcat(finHour, finMin);
		printf("Adjusted local time at process %d is %s\n", rank, finHour); 
		
 }

fclose(fp1);
MPI_Finalize();
return 0;
}