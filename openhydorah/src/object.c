#include "object.h"

#include "filesystem.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

Object* CreateObject(Sprite* sprite, const char* name,
		SDL_Point point, CollisionBox collision, Object* parent,
		Object* children)
{
	Object* obj = malloc(sizeof(Object));
	obj->sprite = sprite;
	if (name != NULL)
	{
		size_t len = strlen(name);
		obj->name = malloc(len + 1);
		strcpy(obj->name, name);
	}
	else
		obj->name = NULL;
	obj->point = point;
	if (parent != NULL)
	{
		obj->point.x += parent->point.x;
		obj->point.y += parent->point.y;
	}
	obj->parent = parent;
	obj->children = children;
	obj->next = NULL;
	AddChildObject(parent, obj);
	obj->collision = collision;
	obj->collision.x += obj->point.x;
	obj->collision.y += obj->point.y;
	
	return obj;
}

void DrawObject(Object* object, SDL_Renderer* renderer)
{
	if (object == NULL) return;

	DrawSpriteAtPoint(object->sprite, object->point, renderer);
	DrawObject(object->children, renderer);
}

void DestroyObject(Object* object)
{
	while (object != NULL)
	{
		DestroyObject(object->children);
		DestroySprite(object->sprite);
		Object* temp = object;
		object = object->next;
		free(temp->name);
		free(temp);
	}
}

void AddChildObject(Object* parent, Object* child)
{
	if (parent == NULL || child == NULL)
		return;

	Object** iter = &(parent->children);
	while (*iter != NULL)
		iter = &((*iter)->next);

	*iter = child;
}

Object* CreateObjectsFromJSON(json_t* root, TextureList** textures, Object* parent)
{
	if (!json_is_array(root))
	{
		SDL_LogError(
				SDL_LOG_CATEGORY_APPLICATION,
				"Can't create objects. JSON is not array."
				);
		json_decref(root);
		return NULL;
	}

	uint32_t i = 0;
	Object* objects = NULL;
	Object** objIter = &objects;
	for (i = 0; i < json_array_size(root); i++)
	{
		json_t* elem = NULL;
		json_t* objJSON = NULL;
		json_t* xJSON = NULL;
		json_t* yJSON = NULL;
		SDL_Point point;

		elem = json_array_get(root, i);
		if (!json_is_object(elem))
		{
			SDL_LogWarn(
					SDL_LOG_CATEGORY_APPLICATION,
					"Element in objects array is not JSON object."
					);
			continue;
		}

		xJSON = json_object_get(elem, "x");
		if (json_is_integer(xJSON))
		{
			point.x = json_integer_value(xJSON);
		}

		yJSON = json_object_get(elem, "y");
		if (json_is_integer(yJSON))
		{
			point.y = json_integer_value(yJSON);
		}

		objJSON = json_object_get(elem, "object");
		if (json_is_string(objJSON))
		{
			(*objIter) = CreateObjectFromFile(json_string_value(objJSON), point, textures, parent);
			objIter = &((*objIter)->next);
		}
		else if(json_is_object(objJSON))
		{
			(*objIter) = CreateObjectFromJSON(objJSON, point, textures, parent);
			objIter = &((*objIter)->next);
		}
		else
		{
			SDL_LogWarn(
					SDL_LOG_CATEGORY_APPLICATION,
					"Unknown object element, for array element, for objects in JSON."
					);
		}
	}

	return objects;
}

Object* CreateObjectFromJSON(json_t* root, SDL_Point point, TextureList** textures, Object* parent)
{
	if (!json_is_object(root))
	{
		SDL_LogError(
				SDL_LOG_CATEGORY_APPLICATION,
				"Can't create object. JSON is not object."
				);
		json_decref(root);
		return NULL;
	}

	json_t* nameJSON = NULL;
	json_t* childrenJSON = NULL;
	json_t* spriteJSON = NULL;
	json_t* collisionJSON = NULL;

	char* name = NULL;
	Object* obj = NULL;
	Sprite* sprite = NULL;
	Object* children = NULL;
	CollisionBox collisionBox;

	nameJSON = json_object_get(root, "name");
	if (json_is_string(nameJSON))
	{
		const char* val = json_string_value(nameJSON);
		name = malloc(strlen(val) + 1);
		strcpy(name, val);
	}

	spriteJSON = json_object_get(root, "sprite");
	if (json_is_string(spriteJSON))
	{
		sprite = CreateSpriteFromFile(json_string_value(spriteJSON), textures);
	}

	collisionJSON = json_object_get(root, "collision");
	if (!json_is_object(collisionJSON))
	{
		SDL_LogError(
				SDL_LOG_CATEGORY_APPLICATION,
				"Object JSON does not contain 'collision'"
				);
		return NULL;
	}

	collisionBox = CreateCollisionBoxFromJSON(collisionJSON);


	childrenJSON = json_object_get(root, "children");
	if (json_is_array(childrenJSON))
	{
		children = CreateObjectsFromJSON(childrenJSON, textures, obj);
	}

	obj = CreateObject(sprite, name, point,
			collisionBox, parent, children);

	return obj;
}

Object* CreateObjectFromFile(const char* filename, SDL_Point point, TextureList** textures, Object* parent)
{
	Object* obj = NULL;
	uint8_t* buf = NULL;
	PHYSFS_sint64 readLength = 0;
	json_t* rootNode = NULL;
	json_error_t error;

	readLength = ReadFileToBuffer(filename, &buf);
	if (readLength == 0)
	{
		SDL_LogError(
				SDL_LOG_CATEGORY_APPLICATION,
				"Got no data from '%s'",
				filename
				);
		return NULL;
	}

	SDL_Log("Reading object - %s - size: %i", filename, readLength);

	rootNode = json_loadb(buf, readLength, 0, &error);
	free(buf);

	if (rootNode == NULL)
	{
		SDL_LogError(
				SDL_LOG_CATEGORY_APPLICATION,
				"JSON error for file '%s' - line: %d - message: %s.",
				filename, error.line, error.text
				);
		return NULL;
	}

	obj = CreateObjectFromJSON(rootNode, point, textures, parent);

	return obj;
}

Object* FindObjectInPoint(Object* root, SDL_Point point)
{
	Object* obj = NULL;
	while (root != NULL)
	{
		obj = FindObjectInPoint(root->children, point);
		if (obj != NULL)
			return obj;

		if (root->point.x < point.x &&
				root->point.x + root->sprite->currentFrame->rect.w > point.x &&
				root->point.y < point.y &&
				root->point.y + root->sprite->currentFrame->rect.h > point.y
		   )
			return root;

		root = root->next;
	}

	return NULL;
}

void DrawObjectCollisions(Object* object, SDL_Renderer* renderer)
{
	while (object != NULL)
	{
		DrawCollisionBox(object->collision, renderer);
		DrawObjectCollisions(object->children, renderer);

		object = object->next;
	}
}

Object* FindObjectByName(Object* root, const char* name)
{
	while (root != NULL)
	{
		if (strcmp(root->name, name) == 0)
			return root;

		Object* childObj = FindObjectByName(root->children, name);
		if (childObj != NULL)
			return childObj;
	}
	
	return NULL;
}

void DestroyObjectList(ObjectList* list)
{
	while (list != NULL)
	{
		ObjectList* temp = list;
		list = list->next;
		free(temp);
	}
}

ObjectList* FindObjectsInRect(Object* root, SDL_Rect rect)
{
	ObjectList* list = NULL;
	ObjectList** iter = &list;

	while (root != NULL)
	{
		if (root->sprite == NULL || root->sprite->currentFrame == NULL)
		{
			root = root->next;
			continue;
		}

		SDL_Point halfSize;
		halfSize.x = root->sprite->currentFrame->rect.w / 2;
		halfSize.y = root->sprite->currentFrame->rect.h / 2;

		SDL_Point point;
		point.x = root->point.x + halfSize.x;
		point.y = root->point.y + halfSize.y;

		SDL_Rect rectEx;
		rectEx.x = rect.x - halfSize.x;
		rectEx.y = rect.y - halfSize.y;
		rectEx.w = rect.w + halfSize.x;
		rectEx.h = rect.h + halfSize.y;

		if (rectEx.x <= point.x && rect.x + rectEx.w >= point.x &&
				rectEx.y <= point.y && rect.y + rectEx.h >= point.y)
		{
			*iter = malloc(sizeof(ObjectList));
			(*iter)->object = root;
			(*iter)->next = NULL;
			iter = &((*iter)->next);
		}
		root = root->next;
	}

	return list;
}
