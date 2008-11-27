#include <stdio.h>
#include <TCFoundation/mystring.h>

int Headerize(const char *srcFilename)
{
	FILE *srcFile = fopen(srcFilename, "rb");

	if (srcFile)
	{
		char *baseFilename = filenameFromPath(srcFilename);
		char *dotSpot = strrchr(baseFilename, '.');
		char *dstFilename;
		FILE *dstFile;

		if (dotSpot)
		{
			*dotSpot = 0;
		}
		dstFilename = copyString(baseFilename, 2);
		strcat(dstFilename, ".h");
		dstFile = fopen(dstFilename, "w");
		fseek(srcFile, 0, SEEK_END);
		if (dstFile)
		{
			long fileSize = ftell(srcFile);
			TCByte *bytes = new TCByte[fileSize];

			fseek(srcFile, 0, SEEK_SET);
			fread(bytes, fileSize, 1, srcFile);
			fprintf(dstFile, "#ifndef __%s_h__\n", baseFilename);
			fprintf(dstFile, "#define __%s_h__\n\n", baseFilename);
			fprintf(dstFile, "unsigned char %s_bytes[] =\n", baseFilename);
			fprintf(dstFile, "{");
			for (int i = 0; i < fileSize; i++)
			{
				const char *comma = ",";

				if (i == fileSize - 1)
				{
					comma = "\n";
				}
				if (i % 16 == 0)
				{
					fprintf(dstFile, "\n\t");
				}
				fprintf(dstFile, "%3d%s", bytes[i], comma);
			}
			fprintf(dstFile, "};\n\n");
			fprintf(dstFile, "#endif // __%s_h__\n", baseFilename);
			fclose(dstFile);
		}
		fclose(srcFile);
	}
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc == 2)
	{
		return Headerize(argv[1]);
	}
	else
	{
		printf("Usage: Headerize <input filename>\n");
		return 1;
	}
}
