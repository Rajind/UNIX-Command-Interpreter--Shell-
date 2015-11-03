/*********************************************************************
* FOR LOG MESSAGES
*********************************************************************/

void logMessage(char *message){
	if(logsOn == TRUE){
		printf("%s\n",message);	
	}
}

/*********************************************************************
 * USER INPUT FUNCTIONS
 *********************************************************************/

/**
 *	gets a line of text from user
 */
void getTextLine()
{
        destroyCommand();                                                               // clean buffer and command line
        while ((userInput != '\n') && (bufferChars < BUFFER_MAX_LENGTH)) {
                buffer[bufferChars++] = userInput;
                userInput = getchar();
        }
        buffer[bufferChars] = 0x00;                                                     // null terminate buffer
        
	//parseCommand();
	//populateCommand();
	logMessage("end getTextLine");
}


/*
* parses set of commands 	
*
*/

void parseCommand(){
	char* bufferPointer;                                                            // a pointer to the buffer
        bufferPointer = strtok(buffer, "&&");
        while (bufferPointer != NULL) {                                                 // while the pointer is not NULL
                command[commandC] = bufferPointer;                   		   	// populate commandC array		
		bufferPointer = strtok(NULL, "&&");                
		commandC++;
        }	
}

/**
 * populates commandArgv array with every words
 * of the command line (space " " used as separator)
 */

void populateCommand(){
        char* bufferPointer;                                                               // a pointer to the buffer
        bufferPointer = strtok(buffer, " ");
        while (bufferPointer != NULL) {                                                    // while the pointer is not NULL
                commandArgv[commandArgc] = bufferPointer;                   		   // populate commandArgv array		
		bufferPointer = strtok(NULL, " ");                
		commandArgc++;
        }
		
	if(strcmp("&", commandArgv[commandArgc - 1]) == 0){			
		int i = commandArgc;
		char *p;
		while(i > 0){
			commandArgv[i] = commandArgv[i-1];
			i--;
		}			
		commandArgv[0] = "bg";		
	}
}

void populateMultipleCommand(char *commandBuffer)
{
        char* bufferPointer;                                                               // a pointer to the buffer
        bufferPointer = strtok(commandBuffer, " ");
        while (bufferPointer != NULL) {                                                    // while the pointer is not NULL
                commandArgv[commandArgc] = bufferPointer;                   		   // populate commandArgv array	
		bufferPointer = strtok(NULL, " ");                
		commandArgc++;
        }
		
	if(strcmp("&", commandArgv[commandArgc - 1]) == 0){			
		int i = commandArgc;
		char *p;
		while(i > 0){
			commandArgv[i] = commandArgv[i-1];
			i--;
		}			
		commandArgv[0] = "bg";		
	}
}

/**
 *	destroys the arrays of chars, letting the user to input the next command line
 */

void destroyCommand()
{
        while (commandArgc != 0) {
                commandArgv[commandArgc] = NULL;                                 // delete the pointer to the string
                commandArgc--;                                                   // decrement the number of words in the command line
        }

	while (commandC != 0) {
                command[commandC] = NULL;                                 	// delete the pointer to the string
                commandC--;                                                   	// decrement the number of commands
        }
	commandConsecutive = 0;
	
        bufferChars = 0;                                                         // erase the number of chars in the buffer
}

/*********************************************************************
 * FUNCTIONS REGARDING ACTIVE JOBS
 *********************************************************************/

/**
 *	inserts an active process in the linked list
 */
t_job* insertJob(pid_t pid, pid_t pgid, char* name, char* descriptor,
                 int status)
{
        usleep(10000);
        t_job *newJob = malloc(sizeof(t_job));

        newJob->name = (char*) malloc(sizeof(name));
        newJob->name = strcpy(newJob->name, name);
        newJob->pid = pid;
        newJob->pgid = pgid;
        newJob->status = status;
        newJob->descriptor = (char*) malloc(sizeof(descriptor));
        newJob->descriptor = strcpy(newJob->descriptor, descriptor);
        newJob->next = NULL;

        if (jobsList == NULL) {
                numActiveJobs++;
                newJob->id = numActiveJobs;
                return newJob;
        } else {
                t_job *auxNode = jobsList;
                while (auxNode->next != NULL) {
                        auxNode = auxNode->next;
                }
                newJob->id = auxNode->id + 1;
                auxNode->next = newJob;
                numActiveJobs++;
                return jobsList;
        }
}

/**
 * modifies the status of a job
 */
int changeJobStatus(int pid, int status)
{
        usleep(10000);
        t_job *job = jobsList;
        if (job == NULL) {
                return 0;
        } else {
                int counter = 0;
                while (job != NULL) {
                        if (job->pid == pid) {
                                job->status = status;
                                return TRUE;
                        }
                        counter++;
                        job = job->next;
                }
                return FALSE;
        }
}

/**
 *	deletes a no more active process from the linked list
 */
t_job* delJob(t_job* job)
{
        usleep(10000);
        if (jobsList == NULL)
                return NULL;
        t_job* currentJob;
        t_job* beforeCurrentJob;

        currentJob = jobsList->next;
        beforeCurrentJob = jobsList;

        if (beforeCurrentJob->pid == job->pid) {

                beforeCurrentJob = beforeCurrentJob->next;
                numActiveJobs--;
                return currentJob;
        }

        while (currentJob != NULL) {
                if (currentJob->pid == job->pid) {
                        numActiveJobs--;
                        beforeCurrentJob->next = currentJob->next;
                }
                beforeCurrentJob = currentJob;
                currentJob = currentJob->next;
        }
        return jobsList;
}

/**
 * searches a job in the active jobs list, by pid, job id, job status
 */
t_job* getJob(int searchValue, int searchParameter)
{
        usleep(10000);
        t_job* job = jobsList;
        switch (searchParameter) {
        case BY_PROCESS_ID:
                while (job != NULL) {
                        if (job->pid == searchValue)
                                return job;
                        else
                                job = job->next;
                }
                break;
        case BY_JOB_ID:
                while (job != NULL) {
                        if (job->id == searchValue)
                                return job;
                        else
                                job = job->next;
                }
                break;
        case BY_JOB_STATUS:
                while (job != NULL) {
                        if (job->status == searchValue)
                                return job;
                        else
                                job = job->next;
                }
                break;
        default:
                return NULL;
                break;
        }
        return NULL;
}

/**
 * prints the active processes launched by the shell
 */
void printJobs()
{
        printf("\nActive jobs:\n");
        printf(
                "---------------------------------------------------------------------------\n");
        printf("| %7s  | %30s | %5s | %10s | %6s |\n", "job no.", "name", "pid",
               "descriptor", "status");
        printf(
                "---------------------------------------------------------------------------\n");
        t_job* job = jobsList;
        if (job == NULL) {
                printf("| %s %62s |\n", "No Jobs.", "");
        } else {
                while (job != NULL) {
                        printf("|  %7d | %30s | %5d | %10s | %6c |\n", job->id, job->name,
                               job->pid, job->descriptor, job->status);
                        job = job->next;
                }
        }
        printf(
                "---------------------------------------------------------------------------\n");
}

/*********************************************************************
 * PROMPT AND ERROR FUNCTIONS
 *********************************************************************/

/**
 *	displays a welcome screen
 */
void welcomeScreen()
{
        printf("\n-------------------------------------------------\n");
        printf("\tWelcome to myshell version %s\n", SHELL_VERSION);
        printf("\tThis is myshell with process id: %d\n", (int) MYSH_PID);
        printf("-------------------------------------------------\n");
        printf("\n\n");
}

/**
 *	displays a nice prompt for the shell
 */
void shellPrompt()
{
        printf("%s - %s j: %d> ", getenv("LOGNAME"),
               getcwd(currentDirectory, 1024), numActiveJobs);                   // nice shellPrompt with current user and current dir
}
