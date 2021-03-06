#include "init.h"

int InitFS(const char* arg0)
{
	PHYSFS_init(arg0);
	if (PHYSFS_mount("assets", NULL, 0) == 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
				"Failed to mount assets/ folder. %s\n",
				PHYSFS_getLastError()
				);

		return 1;
	}

	if (PHYSFS_mount("assets/graphics/sprites", "/graphics/sprites", 1) == 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
				"Failed to mount assets/graphics/sprites folder. %s\n",
				PHYSFS_getLastError()
				);

		return 1;
	}

	if (PHYSFS_mount("assets/graphics/backgrounds", "/graphics/backgrounds", 1) == 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
				"Failed to mount assets/graphics/backgrounds folder. %s\n",
				PHYSFS_getLastError()
				);

		return 1;
	}

	if (PHYSFS_mount("assets/fonts", "/fonts", 1) == 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
				"Failed to mount assets/fonts folder. %s\n",
				PHYSFS_getLastError()
				);

		return 1;
	}

	if (PHYSFS_mount("assets/sounds", "/sounds", 1) == 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
				"Failed to mount assets/sounds folder. %s\n",
				PHYSFS_getLastError()
				);

		return 1;
	}

	if (PHYSFS_mount("assets/musics", "/musics", 1) == 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
				"Failed to mount assets/musics folder. %s\n",
				PHYSFS_getLastError()
				);

		return 1;
	}

	if (PHYSFS_mount("assets/sprites", "/sprites", 1) == 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
				"Failed to mount assets/sprites folder. %s\n",
				PHYSFS_getLastError()
				);

		return 1;
	}

	if (PHYSFS_mount("assets/mods", "/mods", 1) == 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
				"Failed to mount assets/mods folder. %s\n",
				PHYSFS_getLastError()
				);

		return 1;
	}

	if (PHYSFS_mount("assets/maps", "/maps", 1) == 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
				"Failed to mount assets/maps folder. %s\n",
				PHYSFS_getLastError()
				);

		return 1;
	}

	if (PHYSFS_mount("assets/objects", "/objects", 1) == 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
				"Failed to mount assets/objects folder. %s\n",
				PHYSFS_getLastError()
				);

		return 1;
	}

	return 0;
}

