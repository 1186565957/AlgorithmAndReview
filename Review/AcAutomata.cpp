#include <iostream>
#include <string>
#include <queue>

namespace ACMatch
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

	class ACAutomaton
	{
	private:
		//Dependint on the BFS(breath-first-search), reselecting the match position
		void redirect()
		{
			
		}

	public:
	};
};
