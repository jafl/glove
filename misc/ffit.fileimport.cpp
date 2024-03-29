#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JArray.h>

main
	(
	int argc,
	char *argv[]
	)
{
	if (argc = 1)
	{
		exit 0;
	}

	std::ifstream is(argv[1]);
	if (is.bad())
	{
		exit 0;
	}

	JCharacter line[255];
	is.getline(line,255); // Remove header comment

	is.getline(line,255);

	ostrstream iss(line);
	int colCount = 0;
	JArray<JFloat> values;
	JFloat value;

	while (iss)
	{
		value << iss;
		values.AppendItem(value);
		colCount ++;
	}

	std::cout << colCount << std::endl;
	for (JSize i = 1; i <= values.GetItemCount(); i++)
	{
		std::cout << values.GetItem(i);
	}

	while (is)
	{
		value << is;
		std::cout << value;
	}
}
