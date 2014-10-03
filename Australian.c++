
#include <iostream> // endl, istream, ostream
#include <vector> // vector
#include <list> // list

class Ballot
{
    public:
        int index;
        std::vector<int> votes;
    
    Ballot()
    {
        index = 0;
    }
};


class Candidate
{
    public:
        int candidateNo;
        std::list<Ballot*> ballots;
        std::string name;
};

//Global list of candidates for use in each election
std::vector<Candidate*> currentCandidates;

//This function processes the input from the istream and creates the Candidate objects
int process_candidates(std::istream& r)
{
    int i;
    r >> i;
    if (!r) return 0;
    for(int j; j < i; j++)
    {
        //Create the Candidate object
        Candidate* candidate = new Candidate();
        
        //Read the candidates name from the istream
        getline(r, candidate->name);
        
        //Add the candidate to the list of candidates
        currentCandidates.push_back(candidate);
    }
    return i;
}

//This function processes the input from the istream and creates the Ballot objects
void process_ballots(std::istream& r, int candidates)
{
    while(r.peek() != '\n')
    {
        Ballot* ballot = new Ballot();
        for(int i = 0; i < candidates; i++)
        {
            int k;
            r >> k;
            ballot->votes.push_back(k);
        }
        int pick = ballot->votes[0];
        currentCandidates[pick]->ballots.push_back(ballot);
    }
}

void Australian_solve(std::istream& r, std::ostream& w)
{
    //Read the number of election cycles to run
    int numElections;
    r >> numElections;
    
    for(int i = 0; i < numElections; i++)
    {
       std::string s;
       std::getline(r, s); //We want to ignore the blank line that happens after each election and after the number of elections
       
       int numCandidates = process_candidates(r);
       process_ballots(r, numCandidates);
       
       //process_election(r, numCandidates);
    }
}

int main()
{
    return 1;
}