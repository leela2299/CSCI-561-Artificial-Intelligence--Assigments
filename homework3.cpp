#include <iostream>
#include <bits/stdc++.h>

using namespace std;

struct data  //data structure for the each predicate
{
    vector<int> sentence; //sentence number
    vector<int> index; //  index of that predicate in that sentence
    vector<bool> is_positive; // sign of that predicate
    vector<vector<string>> arg; // arguments of that predicate in that sentence
};

struct cnf
{
    vector<string> predicate;
    vector<bool> sign;
    vector<vector<string>> arguments;
};

struct resolve
{
    string res_predicate;
    vector<string> res_arg;
    bool res_sign;
};

struct vis
{
    bool is_visited;
    bool is_index;
    unordered_map<string,vector<string> > vis_predicate_map;

};
int total = 0;

unordered_map<string,data> predicate_map; //hash map for the each predicate
unordered_map<int,cnf> sentence_map; //hash map for the each sentence in the cnf form
//map< pair<int,int>, vector<vector<string>> > visited_map;

void remove_space(string &s) // remove all the spaces from the string
{
    s.erase(remove_if(s.begin(), s.end(),::isspace), s.end());
}

void predicate_seperate(string s,vector<pair<string,bool> > &pr) // function to seperate all the predicates in the given sentence
{
    int found;
    found = s.find("=>");
    string p,q;
    pair<string,bool> temp;

    if(found!=string::npos) //imnplication is found in the string
    {
        p = s.substr(0,found);
        q = s.substr(found+2);  // after implication predicate

        stringstream t(p);
        string seg;
        vector<string> r;

        while(getline(t,seg,'&'))
        {
            r.emplace_back(seg);
        }

        for(auto it:r) //push p1,p2,p3.......
        {
            if(it[0]=='~') //check for negation and reverse the sign
            {
                it = it.substr(1);
                temp.first = it;
                temp.second = true;
            }

            else
            {
                temp .first = it;
                temp.second = false;

            }


            pr.emplace_back(temp);
        }

        if(q[0]=='~') // check for the negation
        {
            q = q.substr(1);
            temp. first = q;
            temp.second = false;
            pr.emplace_back(temp); // push the string q

        }
        else
        {
            temp. first = q;
            temp.second = true;
            pr.emplace_back(temp); // push the string q

        }



    }

    else  //only one predicate is there
    {

        if(s[0]=='~') // if negation is found
        {
            s = s.substr(1);
            temp.first = s;
            temp.second = false;
            pr.emplace_back(temp);
        }
        else // if negation is not found
        {
            temp.first = s;
            temp. second = true;
            pr.emplace_back(temp);

        }

    }

}

vector<string> arg_seperate(string s) // return only the arguments
{
    int found;
    vector<string> res;
    found = s.find('(');
    s = s.substr(found+1); //remove the opening bracket
    s.erase(s.end()-1); // remove the closing bracket
    stringstream t(s);
    string seg;

    while(getline(t,seg,','))  //remove the commas
    {
        res.emplace_back(seg);
    }

    return res;
}



bool is_constant(string s)
{
    if(isupper(s[0])) return true;
    else return false;
}

bool is_matched(vector<vector<string>> s,vector<string> curr)

{
    int i,j;
    vector<string> temp;
    for(i=0;i<s.size();i++)
    {
        temp = s[i];
        for(j=0;j<temp.size();j++)
        {
            if(curr[j] != temp[j])
            {
                break;
            }
        }

        if(j==temp.size()) return true;
    }

    return false;
}


bool argument_unification(vector<string> carg,vector<string> varg,unordered_map<string,string> &argument_match)
{
    int i;

    for(i=0;i<carg.size();i++)
    {

        //case 1 both are constants
        if(is_constant(carg[i]) and is_constant(varg[i]))
        {
            if(carg[i]!=varg[i]) //both constants are not equal then unifcation does not happens
            {
                return false;
            }
        }

        //case 2 both are variables

        if( (not(is_constant(carg[i]))) and (not(is_constant(varg[i]))) )
        {
            return false;
        }


        // case3 carg is constant and varg is variable

        if((is_constant(carg[i])) and not(is_constant(varg[i])))
        {
            argument_match[varg[i]] = carg[i];
        }

        //case4 carg is variable and varg is constant

        if( (is_constant(varg[i])) and not(is_constant(carg[i])) )
        {
            argument_match[carg[i]] = varg[i];
        }
    }

    return true;
}

vector<string> equate_arg(unordered_map<string,string> &argument_match,vector<string> s)
{
    vector<string> res;
    int i;
    for(auto it:s)
    {
        if(is_constant(it))
        {
            res.emplace_back(it);
        }
        else
        {
            if(argument_match.find(it) != argument_match.end())
            {
                res.emplace_back(argument_match[it]); //if that variable has a matching
            }

            else
            {
                res.emplace_back(it); //if that variables does not unify with the constant
            }
        }
    }

    return res;
}

void update(vector<resolve> &resolve_form,int s_no,int ind,unordered_map<string,string> &argument_match)
{
    cnf curr_cnf;
    curr_cnf = sentence_map[s_no];
    resolve temp_res;
    //cout<<"size is"<< curr_cnf.predicate.size();


    int i,j;
    for(i=0;i<resolve_form.size();i++) // for each predicate in resolve form
    {
        for(j=0;j<resolve_form[i].res_arg.size();j++) // for each literal in the predicate
        {
            if(not(is_constant(resolve_form[i].res_arg[j]))) //if that particular literal is not constant
            {
                if(argument_match.find(resolve_form[i].res_arg[j]) != argument_match.end()) // if there is mapping for that variable
                {
                    resolve_form[i].res_arg[j] = argument_match[resolve_form[i].res_arg[j]];
                }
            }
        }
    }


    for(i=0;i<curr_cnf.predicate.size();i++) //iterate over the sentence map
    {
        if(i!=ind)  //check whether both the indexes are same or not
        {
            temp_res.res_predicate = curr_cnf.predicate[i]; // filling predicate
            temp_res.res_sign = curr_cnf.sign[i]; // filling sign
            temp_res.res_arg = equate_arg(argument_match,curr_cnf.arguments[i]); //update the temp_res
            resolve_form.emplace_back(temp_res);

            //you have to do something here leela that is the place where everything happens

        }
    }

}


vector<vector<resolve>> is_unify(vector<resolve> resolve_form,resolve curr_res,int r,bool &is_unification)
{
    string curr_predicate = curr_res.res_predicate;
    bool curr_sign = curr_res.res_sign;
    vector<string> curr_arg = curr_res.res_arg;
    int i,j;
    map< pair<int,int>, vector<vector<string>> > visited_map; // unique pair of sentecnce_number and index_number. value is the arguments that have been visited already
    pair<int,int> p;
    data curr_data;
    unordered_map<string,string> argument_match; // to store the map of variables and constants
    vector<vector<string>> temp_arg;
    vector<resolve>::iterator it;
    is_unification = false;
    vector<resolve> temp_resolve_form; //to store and update the resolve form
    vector<vector<resolve>> final_resolve;

    int number;



    if(predicate_map.find(curr_predicate)==predicate_map.end()) // if there is no such predicate in the knowledge base
    {
        is_unification = false;
        return final_resolve;
    }

    else //such predicate exist in the map
    {
        curr_data = predicate_map[curr_predicate]; // intialize it as the curr_predicate



        for(i=0;i<curr_data.sentence.size();i++)
        {
            p.first = curr_data.sentence[i];
            p.second = curr_data.index[i];
            argument_match.clear(); // clear the argument match clear

            if(resolve_form.size()==1) // if there is only predicate to unify then we have this at here
            {
                if(sentence_map[p.first].predicate.size()!=0)
                {
                    if(curr_data.is_positive[i]==not(curr_sign)) //check if both are of different signs for the unification
                    {
                        if(argument_unification(curr_arg,curr_data.arg[i],argument_match)) // check for the argument
                        {
                            p.first =  curr_data.sentence[i];
                            p.second =  curr_data.index[i];

                            if(visited_map.find(p)==visited_map.end()) //not visited already
                            {
                                temp_arg.emplace_back(curr_arg);
                                visited_map[p] = temp_arg;
                                temp_arg.clear();//clear the vector
                                //update the resolve form
                                temp_resolve_form = resolve_form;
                                temp_resolve_form.erase(temp_resolve_form.begin() + r);
                                update(temp_resolve_form, p.first, p.second, argument_match);
                                is_unification = true; //unification is possible
                                final_resolve.emplace_back(temp_resolve_form); //push the current resolve into the vector
                                temp_resolve_form.clear(); //clear the temp_resolve_form
                            }

                            else
                            {
                                if(is_matched(visited_map[p],curr_arg)) //check whether the current arguments already matched or not
                                {
                                    continue;
                                }
                                else
                                {
                                    visited_map[p].emplace_back(curr_arg);
                                    //update the resolve form
                                    temp_resolve_form = resolve_form;
                                    temp_resolve_form.erase(temp_resolve_form.begin() + r);
                                    update(temp_resolve_form, p.first, p.second, argument_match);
                                    is_unification = true; //unification is possible
                                    final_resolve.emplace_back(temp_resolve_form); //push the current resolve into the vector
                                    temp_resolve_form.clear(); //clear the temp_resolve_form
                                }
                            }
                        }
                    }
                }

            }
            else   // look for all the sentences with size 1
            {
                if(sentence_map[p.first].predicate.size()==1)
                {
                    if(curr_data.is_positive[i]==not(curr_sign)) //check if both are of different signs for the unification
                    {
                        if(argument_unification(curr_arg,curr_data.arg[i],argument_match)) // check for the argument
                        {
                            p.first =  curr_data.sentence[i];
                            p.second =  curr_data.index[i];

                            if(visited_map.find(p)==visited_map.end()) //not visited already
                            {
                                temp_arg.emplace_back(curr_arg);
                                visited_map[p] = temp_arg;
                                temp_arg.clear();//clear the vector
                                //update the resolve form
                                temp_resolve_form = resolve_form;
                                temp_resolve_form.erase(temp_resolve_form.begin() + r);
                                update(temp_resolve_form, p.first, p.second, argument_match);
                                is_unification = true; //unification is possible
                                final_resolve.emplace_back(temp_resolve_form); //push the current resolve into the vector
                                temp_resolve_form.clear(); //clear the temp_resolve_form
                            }

                            else
                            {
                                if(is_matched(visited_map[p],curr_arg)) //check whether the current arguments already matched or not
                                {
                                    continue;
                                }
                                else
                                {
                                    visited_map[p].emplace_back(curr_arg);
                                    //update the resolve form
                                    temp_resolve_form = resolve_form;
                                    temp_resolve_form.erase(temp_resolve_form.begin() + r);
                                    update(temp_resolve_form, p.first, p.second, argument_match);
                                    is_unification = true; //unification is possible
                                    final_resolve.emplace_back(temp_resolve_form); //push the current resolve into the vector
                                    temp_resolve_form.clear(); //clear the temp_resolve_form
                                }
                            }
                        }
                    }
                }

            }

        }

    }

    return final_resolve;


}


bool resolution(vector<resolve> resolve_form,int count,resolve new_res)
{
    int i,j,k;
    bool is_unification;
    map< pair<int,int>, vector<vector<string>> > visited_map;
    unordered_map<string,string> argument_match; //remember to clean the argument_match map for each new unification
    string curr_predicate;
    bool curr_sign;
    vector<string> curr_arg;
    vector<resolve> temp_resolve_form;
    pair<int,int> p;
    data curr_data;

    if(count == 100) return false; //infinte loop

    if(resolve_form.size()==0) return true; //statemnt has been proved

    count++; //increase the count
    total++;
    // cout<<count<<" ";

    for(i=0;i<resolve_form.size();i++) //iterating for the each predicate in the resolution form
    {
        curr_predicate = resolve_form[i].res_predicate;
        curr_sign = resolve_form[i].res_sign;
        curr_arg = resolve_form[i].res_arg;

        // check whether it equals the negation of the query

        if(new_res.res_sign == not(curr_sign))
        {
            if(new_res.res_arg == curr_arg )
            {
                if(new_res.res_predicate == curr_predicate)
                {
                    temp_resolve_form = resolve_form;
                    temp_resolve_form.erase(temp_resolve_form.begin()+i);
                    if(resolution(temp_resolve_form,count,new_res))
                    {
                        //if(count==999)cout<<curr_predicate;
                        return true;
                    }
                }
            }
        }


        if(predicate_map.find(curr_predicate) != predicate_map.end()) // if the predicate is found, then continue
        {
            //curr_data = predicate_map[curr_predicate];
            // cout<<curr_predicate<<" ";
            for(j=0;j<predicate_map[curr_predicate].sentence.size();j++) //iterate through all the sentences of the particular predicate
            {
                //clear the argument argument_match
                argument_match.clear();
                //cout<<"size is"<<curr_data.sentence.size();
                if(not(curr_sign) == predicate_map[curr_predicate].is_positive[j] ) // if both of them have different signs
                {
                    //cout<<"leela";
                    if(argument_unification(curr_arg,predicate_map[curr_predicate].arg[j],argument_match)) //if unification is possible
                    {
                        p.first = predicate_map[curr_predicate].sentence[j]; //sentence number
                        p.second = predicate_map[curr_predicate].index[j]; // index number

                        temp_resolve_form = resolve_form;
                        temp_resolve_form.erase(temp_resolve_form.begin()+i);
                        update(temp_resolve_form,p.first,p.second,argument_match);

                        if(count==96)cout<<resolve_form.size();

                        if(resolution(temp_resolve_form,count,new_res))
                        {


                            return true;
                        }
                    }
                }
            }

        }


//

    }
    //cout<<"leela";




//    while(true)
//    {
//        is_unification = false; //start of the loop declare it as true
//        count++;
//        for(i=0;i<resolve_form.size();i++)
//        {
//            //cout<<"leela";
//            if(is_unify(resolve_form,resolve_form[i],i,visited_map))
//            {
//                is_unification = true;
//
//                if(resolve_form.size()==0) return true;// if resolve_form .size() ==0
//
//                break; // break out of the for loop
//            }
//
//
//
//        }
//
//        if(is_unification==false)
//        {
//            //cout<<count; to check how may times the loop is repeating
//            return false; //no unification has been found
//        }
//
//
//
//    }
//
    if(count==96)
    {
        cout<<count<<"leela surya teja";
    }
    // cout<<count;
    return false;


}

bool literal_match(vector<string> a, vector<string> b)
{
    int i;
    for(i=0;i<a.size();i++)
    {
        if(a[i]!=b[i])
        {
            return false;
        }
    }

    return true;

}

bool another_match(vector<string> c,vector<string> v)
{
    int i;
    for(i=0;i<v.size();i++)
    {
        if(is_constant(v[i]))
        {
            if(v[i]!=c[i]) // both the constants does not match, then return false
            {
                return false;
            }
        }
    }

    return true;
}

bool remaining_duplicate(vector<resolve> resolve_form,int ind)
{
    if(resolve_form.size()==1) return true;

    int i,j,k;
    string temp_predicate;
    bool temp_sign;
    vector<string> temp_arg;
    bool sentence_matching = false;

    for(i=1;i<resolve_form.size();i++)
    {
        temp_predicate = resolve_form[i].res_predicate;
        temp_sign = resolve_form[i].res_sign;
        temp_arg = resolve_form[i].res_arg;

        if(predicate_map.find(temp_predicate) == predicate_map.end()) //if that predicate is not found in the map
        {
            return false;
        }

        else
        {
            sentence_matching = false;
            for(j=0;j<predicate_map[temp_predicate].sentence.size();j++)
            {
                if(ind == predicate_map[temp_predicate].sentence[j]) //if the sentence number mathces
                {
                    if(temp_sign == predicate_map[temp_predicate].is_positive[j]) //check for the sign matching
                    {
                        if(literal_match(temp_arg,predicate_map[temp_predicate].arg[j])) // if both the literals also match exactly
                        {
                            // do not do anything here
                        }
                        else
                        {
                            return false; //literals have not matched
                        }
                    }

                    else
                    {
                        return false; //sign has not been matched
                    }

                    sentence_matching = true;
                }

            }

            if(sentence_matching==false) return false; //that predicate is not in the sentence matching
        }
    }

    return true;

}


bool duplicate(vector<resolve> resolve_form)
{
    int j;
    string temp_predicate;
    bool temp_sign;
    vector<string> temp_arg;
    int s_no;

    // checking for the first predicate
    temp_predicate = resolve_form[0].res_predicate;
    temp_sign = resolve_form[0].res_sign;
    temp_arg = resolve_form[0].res_arg;

    if(predicate_map.find(temp_predicate)==predicate_map.end()) // if that predicate is not found
    {
        return false;
    }

    for(j=0;j<predicate_map[temp_predicate].sentence.size();j++) //check for all the sentences
    {

        s_no = predicate_map[temp_predicate].sentence[j];
        if(sentence_map[s_no].predicate.size()==resolve_form.size()) // if the size of sentence and resolve form are same
        {
            if(temp_sign == predicate_map[temp_predicate].is_positive[j]) //check for the sign matching
            {
                if(literal_match(temp_arg,predicate_map[temp_predicate].arg[j])) // if both the literals also match exactly
                {
                    if(remaining_duplicate(resolve_form,s_no))
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;


}

void dfs(vector<resolve> resolve_form,int count)
{
    count++;

    int i,j;
    bool is_unification=false;
    vector<vector<resolve>> resolve_set;
    vector<resolve> temp_resolve_form;
    string temp_predicate;
    vector<string> temp_arg;
    bool temp_sign;
    int s_no;

    for(i=0;i<resolve_form.size();i++) //check whether resolution is possible and add them to the kb
    {
        resolve_set = is_unify(resolve_form,resolve_form[i],i,is_unification);
        if(is_unification)
        {

            break;
        }
    }

    if(is_unification==false) return; //return if no unification happend

    for(i=0;i<resolve_set.size();i++)
    {
        if(resolve_set[i].size()==0)
        {
            continue;
        }
        temp_resolve_form = resolve_set[i];
        s_no = sentence_map.size(); // take the size of sentence_map
        if(duplicate(resolve_set[i]))
        {
            continue;
        }

        for(j=0;j<temp_resolve_form.size();j++)
        {
            cnf temp_cnf; //for the sentence_map
            data temp_data; //for the predicate_map

            temp_predicate = temp_resolve_form[j].res_predicate;
            temp_sign = temp_resolve_form[j].res_sign;
            temp_arg = temp_resolve_form[j].res_arg;

            //--------- update the sentence_map------------------------------------------
            if(sentence_map.find(s_no)==sentence_map.end())
            {
                temp_cnf.predicate.emplace_back(temp_predicate);
                temp_cnf.sign.emplace_back(temp_sign);
                temp_cnf.arguments.emplace_back(temp_arg);
                sentence_map[s_no] = temp_cnf;
            }

            else
            {
                sentence_map[s_no].predicate.emplace_back(temp_predicate);
                sentence_map[s_no].sign.emplace_back(temp_sign);
                sentence_map[s_no].arguments.emplace_back(temp_arg);
            }

            //---------update the predicate_map too -------------------------------------------

            if(predicate_map.find(temp_predicate)==predicate_map.end()) // not in the map
            {
                temp_data.sentence.emplace_back(s_no);
                temp_data.index.emplace_back(j);
                temp_data.is_positive.emplace_back(temp_sign);
                temp_data.arg.emplace_back(temp_arg);
                predicate_map[temp_predicate] = temp_data;

            }

            else // if the predicate is already in the map
            {
                predicate_map[temp_predicate].sentence.emplace_back(s_no);
                predicate_map[temp_predicate].index.emplace_back(j);
                predicate_map[temp_predicate].is_positive.emplace_back(temp_sign);
                predicate_map[temp_predicate].arg.emplace_back(temp_arg);

            }


        }
        //cout<<sentence_map.size();
        dfs(resolve_set[i],count); //update the predicate_map and sentece_map before calling the function of dfs
        //if(i==0) break;
    }
}



int main()
{
    ifstream fin;
    ofstream fout;
    fin.open("input.txt");
    fout.open("output.txt");
    int no_queries,no_kb,i,j;
    char temp[10000];
    string s;
    vector<string> queries,kb;
    fin.getline(temp,10000);
    s = temp;
    remove_space(s);
    no_queries = stoi(s); //read the no of queries
    int s_no;
    bool res=false;

    for(i=0;i<no_queries;i++) //store the queries
    {
        fin.getline(temp,10000);
        s = temp;
        remove_space(s);
        queries.emplace_back(s);
    }
    fin.getline(temp,10000);
    s = temp;
    remove_space(s);
    no_kb = stoi(s); // read the no of sentences in kb

    for(i=0;i<no_kb;i++) //store the queries
    {
        fin.getline(temp,10000);
        s = temp;
        remove_space(s);
        kb.emplace_back(s);
    }

    //----------------------------------------------------------------------------------------------------------------------------------------

    vector<pair<string,bool>> pr; // to store the predicates
    int k;
    resolve temp_res;
    vector<resolve> resolve_form;

    // predicate_seperate(kb[0],pr);

    for(i=0;i<no_kb;i++)
    {
        predicate_seperate(kb[i],pr);
        vector<string> temp_arg; // to store the all the arguments of the given predicate;
        string temp_predicate;
        int f;
        s_no = sentence_map.size();
        for(k=0;k<pr.size();k++)
        {

            data temp_data; //struct of data
            cnf temp_cnf; //struct of cnf
            temp_arg = arg_seperate(pr[k].first); // pass the predciate into the function
            temp_predicate = pr[k].first; //predicate name
            f = temp_predicate.find('(');
            temp_predicate = temp_predicate.substr(0,f); //only the string of predicate name


            //----fill the sentence_map--------------------
            // should insert sentence_first and sentence no changes----------
            if(sentence_map.find(s_no)==sentence_map.end())
            {
                temp_cnf.predicate.emplace_back(temp_predicate);
                temp_cnf.sign.emplace_back(pr[k].second);
                temp_cnf.arguments.emplace_back(temp_arg);
                sentence_map[s_no] = temp_cnf;
            }

            else
            {
                sentence_map[s_no].predicate.emplace_back(temp_predicate);
                sentence_map[s_no].sign.emplace_back(pr[k].second);
                sentence_map[s_no].arguments.emplace_back(temp_arg);
            }

            //------ find the predciate_map----------
            if(predicate_map.find(temp_predicate)==predicate_map.end()) // not in the map
            {
                temp_data.sentence.emplace_back(s_no);
                temp_data.index.emplace_back(k);
                temp_data.is_positive.emplace_back(pr[k].second);
                temp_data.arg.emplace_back(temp_arg);
                predicate_map[temp_predicate] = temp_data;

            }

            else // if the predicate is already in the map
            {
                predicate_map[temp_predicate].sentence.emplace_back(s_no);
                predicate_map[temp_predicate].index.emplace_back(k);
                predicate_map[temp_predicate].is_positive.emplace_back(pr[k].second);
                predicate_map[temp_predicate].arg.emplace_back(temp_arg);

            }


            //pr[k].second is the sign of the query or the sentence.
            temp_res.res_arg = temp_arg;
            temp_res.res_sign = pr[k].second;
            temp_res.res_predicate = temp_predicate;

            resolve_form.emplace_back(temp_res);



            temp_arg.clear(); //clear the vector of temp_arg

        }

        if(i>0)
        {
            //call the recursive dfs that adds sentence to the kb
            dfs(resolve_form,0);
        }


        pr.clear(); // clear the vector pr
        resolve_form.clear(); //clear the resolve form the given query
    }

//    for(auto it:sentence_map)
//    {
//        cout<<it.first<<endl;
//        for(i=0;i<it.second.predicate.size();i++)
//        {
//            cout<<"predicate"<<" "<<it.second.predicate[i]<<" "<<endl;
//            cout<<"sign"<<" "<<it.second.sign[i]<<" "<<endl;
//            for(auto m:it.second.arguments[i])
//            {
//                cout<<m<<",";
//            }
//            cout<<endl;
//            cout<<"......................";
//            cout<<endl;
//        }
//        cout<<endl;
//        cout<<"//////////////////////";
//        cout<<endl;
//    }




    // vector<resolve> resolve_form;

//************************************* resolving the queries*******************************************************************************
    for(i=0;i<no_queries;i++)
    {
        resolve temp_res,new_res;
        string ss = queries[i];
        int t;
        if(ss[0]=='~') //store the sign of the query
        {
            temp_res.res_sign = false;
            new_res.res_sign = false;
            ss = ss.substr(1); //remove the negation symbol
        }
        else
        {
            temp_res.res_sign = true;
            new_res.res_sign = true;
        }

        // get the arguments inside the brackets


        temp_res.res_arg = arg_seperate(ss); // store the arguments of the predicate
        new_res.res_arg = arg_seperate(ss);  // store the arguments of the predicate

        t = ss.find('(');

        temp_res.res_predicate = ss.substr(0,t); //store the predicate name
        new_res.res_predicate = ss.substr(0,t); //store the predicate name

        resolve_form.emplace_back(temp_res); // you have got the resolve form

        res = false;
        if(predicate_map.find(temp_res.res_predicate) == predicate_map.end()) // if that predicate is not inferred in the kb
        {
            res = false;
        }
        else // if that predicate is inferred in the kb
        {
            for(j=0;j<predicate_map[temp_res.res_predicate].sentence.size();j++) //check for all the sentences
            {

                s_no = predicate_map[temp_res.res_predicate].sentence[j];
                if(sentence_map[s_no].predicate.size()==1) // if the size of sentence and resolve form are same
                {
                    if(temp_res.res_sign == predicate_map[temp_res.res_predicate].is_positive[j]) //check for the sign matching
                    {
                        if(another_match(temp_res.res_arg,predicate_map[temp_res.res_predicate].arg[j])) // if both the literals also match exactly
                        {

                            if(i==no_queries-1)
                            {
                                fout<<"TRUE";
                            }
                            else
                            {
                                fout<<"TRUE";
                                fout<<'\n';
                            }
                            res = true;
                            break;
                        }
                    }
                }
            }
        }


        if(res==false)
        {
            if(i==no_queries-1)
            {
                fout<<"FALSE";
            }
            else
            {
                fout<<"FALSE";
                fout<<'\n';
            }
        }
















        resolve_form.clear(); //clear the resolve_form for each query
    }
    //   cout<<sentence_map.size();
//    for(j=0;j<predicate_map["PlayFetch"].sentence.size();j++) //check for all the sentences
//    {
//
//        s_no = predicate_map["PlayFetch"].sentence[j];
//        if(sentence_map[s_no].predicate.size()==1) // if the size of sentence and resolve form are same
//        {
//            for(auto it:predicate_map["PlayFetch"].arg[j])
//            {
//                cout<<it<<" ";
//            }
//
//            cout<<endl;
//        }
//    }





    fout.close();
    fin.close();
    return 0;
}