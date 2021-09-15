#pragma once
#include <Windows.h> 

/*
* DONT USE IT!
* CODE IS NOT OPTIMISED, I WILL UPDATE IT.
* JUST I DONT WANNA TO USE STD/STL, CUZ IT TRASH.
*/

int m_strlen(LPCWSTR lpStr)
{
	if (!lpStr)
		return 0;

	int i = 0;
	while (*lpStr++)
		i++;

	return i;
}

bool m_strcmp(LPCWSTR lpStr1, LPCWSTR lpStr2)
{
	if (!lpStr1 || !lpStr2)
		return false;

	if (m_strlen(lpStr1) != m_strlen(lpStr2))
		return false;

	bool bRes = true;
	while (*lpStr1)
	{
		if (*lpStr1++ != *lpStr2++)
		{
			bRes = false;
			break;
		}
	}

	return bRes;
}

int m_strstr(LPCWSTR lpStr, LPCWSTR lpSubStr)
{
	if (!lpStr || !lpSubStr)
		return -1;

	int iSubStrLen = m_strlen(lpSubStr);
	int i = 0;
	int x = 0;
	int iFirstIndex = -1;

	while (lpStr[x])
	{
		if (lpStr[x] == lpSubStr[i])
		{
			if (!i)
				iFirstIndex = x;

			i++;
			if (i >= iSubStrLen)
			{
				break;
			}
		}
		else
		{
			i = 0;
			iFirstIndex = -1;
		}

		x++;
	}

	return iFirstIndex;
}

struct String
{
protected:
	LPWSTR lpArray = NULL;
	int iSize = 0;
	//

	bool DeallocMemory(LPVOID lpMem)
	{
		return HeapFree(GetProcessHeap(), 0, lpMem);
	}

	LPWSTR AllocMemory(int iSize)
	{
		return (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, iSize);
	}

public:
	void free()
	{
		if (this->lpArray && this->iSize)
			if (DeallocMemory(this->lpArray))
				this->iSize = 0;
	}

	void append(LPCWSTR lpString)
	{
		int iNewString = 0;
		int iSumString = 0;
		int iCurrentSize = m_strlen(this->lpArray);
		LPWSTR lpTempArray = NULL;

		do
		{
			if ((iNewString = m_strlen(lpString)) == 0)
				break;

			iSumString = iCurrentSize + iNewString;
			if ((lpTempArray = AllocMemory(iCurrentSize * 2 + 1)) == NULL)
				break;

			memcpy(lpTempArray, this->lpArray, iCurrentSize * 2);
			if (!DeallocMemory(this->lpArray))
				break;

			if ((this->lpArray = AllocMemory(iSumString * 2 + 1)) == NULL)
				break;

			memcpy(this->lpArray, lpTempArray, iCurrentSize * 2);
			memcpy(&this->lpArray[iCurrentSize], lpString, iNewString * 2);
			this->iSize = iSumString;

		} while (false);

		DeallocMemory(lpTempArray);
	}

	void equate(LPCWSTR lpString)
	{
		int iNewString = 0;
		do
		{
			if (!lpString)
				break;

			iNewString = m_strlen(lpString);

			if (this->lpArray)
				if (!DeallocMemory(this->lpArray))
					break;

			if ((this->lpArray = AllocMemory(iNewString * 2 + 1)) == NULL)
				break;

			memcpy(this->lpArray, lpString, iNewString * 2);
			this->iSize = iNewString;
		} while (false);
	}

	bool equal(LPCWSTR lpString)
	{
		return m_strcmp(this->lpArray, lpString);
	}

	void resize(int iNewSize, int iChar)
	{
		do
		{
			free();

			if ((this->lpArray = AllocMemory(iNewSize * 2 + 1)) == NULL)
				break;

			memset(this->lpArray, iChar, iNewSize * 2);
			this->iSize = iNewSize;
		} while (false);
	}

	bool find(LPCWSTR lpSubString)
	{
		return m_strstr(this->lpArray, lpSubString);
	}

	void erase(LPCWSTR lpSubString)
	{
		int iSubString = 0;
		int iFirstIndex = 0;
		int iLastIndex = 0;
		int iNewSize = 0;
		LPWSTR lpTempArray = NULL;

		do
		{
			if ((iSubString = m_strlen(lpSubString)) == 0)
				break;

			if ((iFirstIndex = m_strstr(this->lpArray, lpSubString)) == -1)
				break;

			iLastIndex = iFirstIndex + iSubString;
			iNewSize = this->iSize - iSubString;
			if ((lpTempArray = AllocMemory(iNewSize * 2 + 1)) == NULL)
				break;

			memcpy(lpTempArray, this->lpArray, iFirstIndex * 2);
			memcpy(&lpTempArray[iFirstIndex], &this->lpArray[iLastIndex], (this->iSize - iLastIndex) * 2);

			equate(lpTempArray);

		} while (false);

		if(lpTempArray)
			DeallocMemory(lpTempArray);
	}

	String &extract(wchar_t wcDelimiter, int iIndex)
	{
		String tempStr;
		int iFirstIndex = -1;
		int iLastIndex = 0;
		int iSkipped = 0;
		int iExtractedLen = 0;
		int i = 0;
		do
		{
			while (this->lpArray[i])
			{
				if (this->lpArray[i] == wcDelimiter)
				{
					if (iSkipped >= iIndex)
					{
						iLastIndex = i;
						break;
					}
					else
					{
						iSkipped++;
						iFirstIndex = -1;
					}
				}
				else
				{
					if (iFirstIndex == -1)
						iFirstIndex = i;
				}

				i++;
			}

			if (iFirstIndex == -1)
				break;

			iExtractedLen = iLastIndex - iFirstIndex;
			tempStr.resize(iExtractedLen, 0);
			memcpy(tempStr.lpArray, &this->lpArray[iFirstIndex], iExtractedLen * 2);

		} while (false);

		return tempStr;
	}

	void int2str(int someInteger, bool bErase)
	{
		int iTemp = someInteger;
		int iTemp2 = someInteger;
		int iChars = 0;
		WCHAR lpTempArr[32] = { 0 };

		while (iTemp /= 10)
			iChars++;

		for (int i = iChars; i >= 0; i--, iTemp2 /= 10)
		{
			lpTempArr[i] = iTemp2 % 10 + '0';
		}

		if (bErase)
			append(lpTempArr);
		else
			equate(lpTempArr);

	}

	int str2int()
	{
		int iRet = 0;
		for (int i = 0; i < this->iSize; i++)
		{
			iRet = (this->lpArray[i] - '0') + (iRet * 10);
		}

		return iRet;
	}

	LPWSTR data()
	{
		return this->lpArray;
	}

	int length()
	{
		return this->iSize;
	}

	//operators

	void operator =(LPCWSTR lpNewString)
	{
		equate(lpNewString);
	}

	void operator +=(LPCWSTR lpNewString)
	{
		append(lpNewString);
	}

	bool operator ==(LPCWSTR lpString)
	{
		return equal(lpString);
	}

	//

	void operator =(String& stringStruct)
	{
		equate(stringStruct.data());
	}

	void operator +=(String& stringStruct)
	{
		append(stringStruct.data());
	}

	bool operator ==(String& stringStruct)
	{
		return equal(stringStruct.data());
	}

	String &operator +(String& stringStruct)
	{
		String newString;
		newString += this->lpArray;
		newString += stringStruct.data();

		return newString;
	}

	//

	void operator =(int someInteger)
	{
		int2str(someInteger, true);
	}

	void operator +=(int someInteger)
	{
		int2str(someInteger, false);
	}

	bool operator ==(int someInteger)
	{
		String tempStr = someInteger;
		return equal(tempStr.data());
	}

	String()
	{
		return;
	}

	String(int iSize, int iChar)
	{
		resize(iSize, iChar);
	}

	String(int someInteger)
	{
		int2str(someInteger, true);
	}

	String(LPCWSTR lpNewString)
	{
		equate(lpNewString);
	}

	String(const String& stringStruct)
	{
		equate(stringStruct.lpArray);
	}

	~String()
	{
		free();
	}
};
