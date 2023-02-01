#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <vector>

#include <algorithm>
#include <functional>

namespace StringMatch
{
	/**
	 * The KMP character-matching algorithm. Effective: O(n)
	 * The essence is the transfer of state-machine
	 * Key-point: keeping the dp-array which is only releated with the flag-str
	 * DP-ARRAY: Value in array is meaning the maximum Length of the substring,
	 * which matched by the reverser order
	 * The next dp-value is depending on the previous value, because the new char is only compared
	 * with the next charactor
	 * 
	 * @return The first starting position of flag-str in string
	*/
	int KMP_strmatch(char *string, uint32_t str_len, char *flag, uint32_t flag_len)
	{
		//All values in dp-array mimnus one. Then the val is according to the compare-idx
		int i = 2, comp_idx = 0;
		int dp[flag_len] = {-1, 0};
		while (i < flag_len)
		{
			if (string[i - 1] == string[comp_idx])

				dp[i++] = ++comp_idx;
			else
			{
				if (comp_idx > 0)
					comp_idx = dp[comp_idx];
				else
					comp_idx = 0;
			}
		}

		//Doing the match progress while the idx is coninclude with the str-end
		int ii, iii, idx;
		while (ii < str_len && iii < flag_len)
		{
			if (string[ii] == flag[iii])
			{
				ii++;
				iii++;
			}
			else
			{
				if (dp[iii] == -1)
					ii++;
				else
					iii = dp[iii];
			}
		}

		//Determine whether the iii has moved to end of the fla-str
		idx = (iii == flag_len ? ii - iii : -1);
		return idx;
	}
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
};
