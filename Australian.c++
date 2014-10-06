// ----------------------------
// Australian\Australian.c++
// Copyright (C) 2014
// Sean A Kruer
// ----------------------------

#include <cassert>  // assert
#include <iostream> // endl, istream, ostream
#include <vector> // vector
#include <list> // list

using namespace std;

//I wrote this whole thing without testing it.

//God help us.

//The Ballot class has two members: An array of the candidates (by number) in order of its preference, 
//and which position it is currently using in that array
class Ballot
{
    public:
        unsigned int index;
        std::vector<int> votes;
    
    Ballot()
    {
        index = 0;
    }
};

//The candidate has three members. Its array index, the list of ballots for which it is currently the
//highest choice, and its name so we can print it later.
class Candidate
{
    public:
        int candidateNo;
        std::list<Ballot*> ballots;
        std::string name;
};

//Global list of candidates for use in each election
std::vector<Candidate*> currentCandidates;

//The total number of votes cast. Yeah, I know, but this is an easy place to keep it.
unsigned int numVotes;

//Candidates left in the running
unsigned int candidatesLeft;

//This function processes the input from the istream and creates the Candidate objects
int process_candidates(std::istream& r)
{
    //cout << "process_candidates" << endl;
    int i;
    r >> i;
    r.get(); //Throw away newline after int
    //cout << "num cadidates: " << i << endl;
    if (!r) return 0;
    for(int j = 0; j < i; j++)
    {
        //cout << "candidate" << j << endl;
        //Create the Candidate object
        Candidate* candidate = new Candidate();
        
        //Read the candidates name from the istream
        //Changing reads to istream reads so that info doesn't have to be immediately ready and I can test on the console
        char name[81];
        r.getline(name, 80);
        candidate->name = name;
        
        //This stores the candidate's position in the vector. This lets us remove
        //candidates that have lost the election.
        candidate->candidateNo = j;
        
        //Add the candidate to the list of candidates
        currentCandidates.push_back(candidate);

        //cout << "Candidate: " << candidate->name << " " << candidate->candidateNo << endl;
    }
    //cout << "end " << endl;
    return i;
}

//This function processes the input from the istream and creates the Ballot objects
void process_ballots(std::istream& r, int candidates)
{
    //cout << "process_ballots" << endl;
    numVotes = 0;
    while(r && r.peek() != '\n')
    {
        ++numVotes;
        Ballot* ballot = new Ballot();
        
        //Read the same number of votes as there are candidates
        for(int i = 0; i < candidates; i++)
        {
            int k;
            r >> k;
            ballot->votes.push_back(k);
            //cout << k << " ";
        }

        //Add this ballot to the list of the Candidate that is their first choice.
        //We have to add one because the array starts at 0.
        int pick = ballot->votes[0] - 1;
        currentCandidates[pick]->ballots.push_back(ballot);
        //cout << currentCandidates[pick]->name << endl;
        
        r.get();
    }
    //cout << numVotes << endl;
}

void find_next_candidate(Ballot* ballot)
{
    //cout << "find_next_candidate" << endl;
    //This line is sort of convoluted. Here's what it's doing:
    //1.Get the current vote "index" (where they are in their list of choices
    //2.Check their list of votes and get the array position of that candidate
    //3.Check if that candidate is still in the race (that is, their pointer != 0)
    //If the pointer is null, repeat. If not, drop out of the loop.
    while(currentCandidates[ballot->votes[ballot->index] - 1] == 0 && (ballot->index < ballot->votes.size() - 1))
    {
        ++(ballot->index);
    }
    assert(ballot->index < ballot->votes.size()); //ensure that we haven't exceeded the bounds. This would be terrible
    
    //Once we have found the new correct position, add this Ballot to that candidate's list.
    currentCandidates[ballot->votes[ballot->index] - 1]->ballots.push_back(ballot);
}

//This function takes an empty list of candidates and fills it with losers
void handle_losers()
{
    //cout << "handle_losers" << endl;
    std::list<Candidate*> losers;
    unsigned int min = 1001; //More votes than any candidate will ever have

    for(Candidate* candidate : currentCandidates)
    {
        // We still have candidates in the array that have removed and set to null, we don't want to do anything with those...
        if(candidate != 0)
        {
            unsigned int size = candidate->ballots.size();
            
            //TODO: adding candidates to this list and then erasing it seems slow. I bet we could do better!
            if(size < min)
            {
                //If candidate has the least votes so far, empty the list of tied candidates, store the new
                //lowest amount of votes, and then add them to the list.
                min = size;
                losers.erase(losers.begin(), losers.end());
            }
            if (size == min)
            {
                //If this is a tie for the least amount of votes, add the new candidate to the list of losers.
                losers.push_back(candidate);
            }
        }
    }
    
    //There are two loops because we want to remove all of the losers before we have ballots reassign their votes.
    for(Candidate* candidate : losers)
    {
        //This line is setting the pointer to this object in the list of all candidates equal to 0.
        //This is how ballots will tell if they can add themselves to a new candidate.
        currentCandidates[candidate->candidateNo] = 0;
        --candidatesLeft;
    }
    
    //This loop makes every ballot for ever loser move itself to a valid candidate.
    for(Candidate* candidate : losers)
    {
        for(Ballot* ballot : candidate->ballots)
        {
            find_next_candidate(ballot);
        }
        //Finally, we want to delete the memory since we're about to get rid of our only reference to it.
        delete candidate;
    }
}

//Checks to see if there is a winner. If there is a winner, we return it. If not, we return a null pointer.
list<Candidate*> check_for_winner()
{
    //cout << "check_for_winner" << endl;
    list<Candidate*> winners;
    unsigned int max = 0;
    for(unsigned int i = 0; i < currentCandidates.size(); i++)
    {
        if(currentCandidates[i] != 0)
        {
            unsigned int vote = currentCandidates[i]->ballots.size();
            //cout << currentCandidates[i]->name << " " << currentCandidates[i]->ballots.size() << endl;
            if(vote > numVotes / 2 + 1)
            {
                winners.push_back(currentCandidates[i]);
                return winners;
            }
            if(vote > max)
                max = vote;
            if(vote == max)
                winners.push_back(currentCandidates[i]);
        }
    }
    if(winners.size() != candidatesLeft)
        winners.clear();

    return winners;
}

//memory management stuff for the remaining candidates when the election ends.
void clean_candidates()
{
    int i =0;
    for(Candidate* candidate : currentCandidates)
    {
        
        if(candidate != 0)
        {
            //cout << candidate->name << endl;
            while(!candidate->ballots.empty())
            {
                delete candidate->ballots.front();
                candidate->ballots.pop_front();
            }

            currentCandidates[i] = 0;
            delete candidate;

        }
        i++;
    }
    currentCandidates.clear();
}

//This is the function that handles the logic loop of running the election.
void process_election(int numCandidates)
{
    //cout << "process_election" << endl;
    for(unsigned int i = 0; i < currentCandidates.size(); i++)
    {
        list<Candidate*> winners = check_for_winner();
        if (winners.size() != 0)
        {
            //Print the name of the winning candidate, followed by an empty line.
            for(Candidate* winner : winners)
            {
                std::cout << winner->name << std::endl;
            }
            return;
        }
        
        //If the election isn't over, then there is more work to do. Handle the list of losers, which
        //means we send their ballots to another candidate and delete them.
        handle_losers();
    }
}

//This is the function that can be called by either the test or the run harness.
void Australian_solve(std::istream& r, std::ostream& w)
{
    //Read the number of election cycles to run
    int numElections;
    r >> numElections;
    //get rid of one newline here
    r.get();
    
    for(int i = 0; i < numElections; i++)
    {
       std::string s;
       //We want to ignore the one newline
       r.get();

       int numCandidates = process_candidates(r);
       candidatesLeft = numCandidates;
       process_ballots(r, numCandidates);

       //An empty space between all testcases, this is my lazy way of figuring out how to do it
       if(i != 0)
            w << endl;

       process_election(numCandidates);

       clean_candidates();
    }
}

int main()
{
    Australian_solve(cin, cout);
    return 0;
}