#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <vector>

#include <algorithm>
#include <functional>

namespace StringMatch
{
	
	/**Same as the KMP-match, By recursivly caculatint the flag-string to 
	 * gain the redirect-table, output-table
	 * 
	 * Unfinite Automata
	 * 
	 *  大致理解就是每一个节点里面存储26个字母的hash表，然后对应的接下来的匹配方式
	 * Goto_table: the next node to be checked , when current node is be fit;
	 * Fail_table: the next node to be checked , when current node isn' t fit; Indexed by the netx-paramter
	 * in structure of ACNode
	 * 
	 * The-Plus: To support Adding or remove flags dynamicly
	*/

#define AC_FAIL_STATE	-1

	typedef struct node
	{
		char charactor;
		
		//hash
		int next;
	} ACNode, *pACNode;

	//For comparint the structure of ACNode
	struct ACNode_finder : public std::binary_function<ACNode, char, bool>
	{
		bool operator() (const ACNode &t, char c)
		{
			return t.charactor == c;
		}
	};

	class ACAutomaton
	{

	private:
		
		std::vector<std::vector<ACNode>> goto_table;	//Corresponding the sider table in the chart
		std::vector<std::vector<std::string>> output_table;
		uint32_t fail_size;
		uint32_t *fail_table;

		//the beginning of spare id
		uint32_t state_id;

	private:
		//Dependint on the BFS(breath-first-search), reselecting the match position
		void redirect()
		{
			
		}

		void resultOutput()
		{

		}

		uint32_t ac_find_goto(int state, char letter)
		{
			std::vector<ACNode>::const_iterator ptr;
			ptr = std::find_if(goto_table[state].begin(), goto_table[state].end(),
							   std::bind2nd(ACNode_finder(), letter));
			
			//not find
			if(ptr == goto_table[state].end())
			{
				// if searching from beginning
				if(state == 0)
					return 0;
				else
					return -1;
			}
			else
				return ptr->next;
		}

		void insertIntoGotoTable(std::string subflag)
		{
			//recurse the letter
			uint32_t state;
			ACNode node;

			std::vector<ACNode> array;
			std::vector<std::string> str;

			std::string::const_iterator ii = subflag.begin();
			for(ii = subflag.begin(); ii < subflag.end(); ii++)
			{
				state = 0;
				uint32_t nextState = ac_find_goto(state, *ii);
				//not find
				if(nextState == 0 || nextState == -1)
				{
					node.charactor = *ii;
					node.next = state_id++;
					goto_table[state].push_back(node);

					//insert the intergate string to table
					goto_table.push_back(array);
					output_table.push_back(str);

					//recursive traversal everyone
					state = node.next;
				}
				else
					state = nextState;
			}

			output_table[state].push_back(subflag);
		}

	public:
		explicit ACAutomaton(uint32_t MaxFlagLen)
		{
			fail_table = nullptr;
			state_id = 0;			
		}

		~ACAutomaton()
		{
			if(fail_table != nullptr)
				free(fail_table);
		}

		//the flag is the filter-strings
		bool AC_create(std::string *flags, uint32_t flag_num)
		{
			if(flags == nullptr || flag_num <= 0)
				return false;

			//adding the node 0
			std::vector<ACNode> array;
			std::vector<std::string> str;
			goto_table.push_back(array);
			output_table.push_back(str);
			state_id++;

			//create the goto_table
			int i;
			for (i = 0; i < flag_num; i++)
			{
				if(flags[i].empty())
					continue;

				insertIntoGotoTable(flags[i]);
			}

			//Initialize the fail-table
			std::queue<int> stateList;

			if(goto_table.empty())
				return false;
			fail_table = (uint32_t *)malloc(sizeof(uint32_t) * goto_table.size());
			if(fail_table == nullptr)
				return false;
			else
				fail_size = goto_table.size();

			//initialize
			for(i=0; i< fail_size; i++)
				fail_table[i] = -1;
					
			int ii;
			//if the last node is zero, when match error go back
			for(ii =0; ii < goto_table[0].size(); ii++)
			{
				fail_table[goto_table[0][ii].next] = 0;
				stateList.push(goto_table[0][ii].next);
			}

			int state, preState, nextState;
			std::vector<ACNode>::const_iterator iter;
			std::vector<std::string>::const_iterator outiter;
			std::vector<std::string>::const_iterator retiter;

			//adding all pending node to stateList
			while (!stateList.empty())
			{
				state = stateList.front();
				stateList.pop();
				for (iter = goto_table[state].begin(); iter <= goto_table[state].end(); iter++)
				{
					if (fail_table[iter->next] != -1) //defined
						continue;
					stateList.push(iter->next);
					preState = state;
					do
					{
						preState = fail_table[preState];
						nextState = ac_find_goto(preState, iter->charactor);
					} while (nextState == -1); //has the next node

					//don' t reach the ending, record the next node in fail-table
					fail_table[iter->next] = nextState;

					//traversal next node' s output-table, 
					//在当前的存储表中找下一跳表的字符串。下一跳肯定是回到更短的位置，如果当前不包含这个更短的字符串就加入，防止不再存在匹配了
					for(outiter = output_table[nextState].begin(); outiter< output_table[nextState].end(); outiter++)
					{
						retiter = std::find(output_table[iter->next].begin(), output_table[iter->next].end(), *outiter);
						if(retiter == output_table[iter->next].end())
							output_table[iter->next].push_back(*outiter);
					}
				}
			}

			return 0;
		}

		//Input data is the string which to be matching
		std::vector<std::pair<int, int>> AC_match(std::string &input)
		{
			if (goto_table.empty())
				return ;

			uint32_t i, idx = 0;
			for(i=0; i< input.size(); i++){
				char letter = input[i];

				while(ac_find_goto(idx, letter) == -1)
					idx = fail_table[idx];

				idx = ac_find_goto(idx, letter);
			}
		}
	};


	/**
	 * Integrating AC and BW algorithms: O(m/n)
	 * 	Matching the string form right to left, if the last charactor isn' t match, stop;
	 * Dismatch: Moving back (bad_idx - last_appear_idx) steps, if no appear-idx the value is -1; Bad-char is in the txt
	 * Different from the last-commend, Whether good-suffix exits. Supporting the mutiple-charater
	 * 
	 * Basic-principle: 
	*/

	///BM-algorithm: 

	class BMMatch
	{
	private: 
		//the max pattern-len support
		uint32_t maxSize;
		const uint32_t asci_len = 256;

		//key is len, value is start_idx
		uint32_t *suffix;
		//bool *isMatch;

		//key is asci, value is idx
		uint32_t *badCharactor;

		typedef enum {
			malloc_err=-3,
			init_err,
			match_err,
			do_err,
			success =0
		}Status;

	public: 
		BMMatch(int MaxPatternLen)
		{
			maxSize = MaxPatternLen > asci_len? MaxPatternLen: asci_len;
			
			suffix = (uint32_t *)malloc(maxSize * sizeof(uint32_t));
			if(suffix == nullptr)
				throw malloc_err;
			memset(suffix, -1, maxSize);

			// isMatch = (bool *)malloc(maxSize * sizeof(bool));
			// if(isMatch == nullptr)
			// 	throw malloc_err;
			// memset(isMatch, 0, maxSize);

			badCharactor = (uint32_t *)malloc(asci_len * sizeof(uint32_t));
			if(badCharactor == nullptr)
				throw malloc_err;
			memset(badCharactor, 0, asci_len);			
		}

		~BMMatch()
		{
			free(suffix);
			free(badCharactor);
			// free(isMatch);
		}
	
	private:
		uint32_t getMoveStep(int errIdx, int pattLen)
		{
			//move by good-suffix
			int idx = pattLen - 1 - errIdx;

			while(idx > 0){
			
			if(suffix[idx] != -1)
				return errIdx - (suffix[idx] -1);
			else
				idx--;
			}

			return pattLen;
		}

	public:
		Status BM_init(const std::string pattern)
		{
			int patt_len = pattern.length();

			//record idx of each charactor in pattern 			
			int i;
			for(i=0; i< maxSize; i++){
				int asci = (int)pattern[i];
				badCharactor[asci] = i;
			}

			for(i = 0; i< pattern.length(); i++){
				int ii = i, len = 0;
				while(i>=0 && pattern[patt_len - 1 - len] == pattern[i]){
					
					suffix[len] = i--;			
					len++;
				}
			}

			return success;
		}

		Status BM_match(const std::string orignal_str, std::string pattern)
		{
			int idx =0;
			int patt_len = pattern.length(), orignal_len = orignal_str.length();
			while(idx < orignal_len - patt_len){

				int i;
				for(i=patt_len-1;)
				
			}


		}


	};
};


