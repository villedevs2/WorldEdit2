#pragma once

#include <glm.hpp>

#include <vector>
#include <string>

#include "Tilemap.h"

class TiledObject
{
public:

private:
};

class Level
{
public:
	enum ObjectType
	{
		OBJECT_TYPE_INVALID			= 0,
		OBJECT_TYPE_COLLISION		= 1,
		OBJECT_TYPE_GEOVIS			= 2,
		OBJECT_TYPE_TRIGGER			= 3,
		OBJECT_TYPE_SLIDER			= 4,
		OBJECT_TYPE_DESTRUCTIBLE		= 5,
		OBJECT_TYPE_MOVER			= 6,
	};

	enum VBOIndex
	{
		VBO_COLLISION = 0,
		VBO_GEOVIS = 1,
		VBO_SLIDER = 2,
		VBO_DESTRUCTIBLE = 3,
		VBO_MOVER = 4,
		NUM_VBOS,
	};

	class Object
	{
		friend class Level;

	public:
		union Param
		{
			int i;
			float f;
			char b[4];
		};

		static const int NUM_PARAMS = 8;

		Object(int id, glm::vec2* points, glm::vec2* uvs, int num_points, ObjectType type, std::string& name);
		~Object();

		bool pointInside(glm::vec2 point, float threshold);
		const glm::vec2& getPoint(int i);
		const glm::vec2& getUV(int i);
		int getNumPoints();
		void insertPoint(glm::vec2 pos, glm::vec2 uv);
		std::string& getName();
		void setName(std::string& name);
		Level::ObjectType getType();
		void setType(Level::ObjectType type);
		int getId();
		void reset();
		void setParam(int index, Level::Object::Param data);
		Level::Object::Param getParam(int index);
		void copy(const Level::Object& source);
		static void setParent(Level* level);
		glm::vec2 getBoundingMin();
		glm::vec2 getBoundingMax();
		void setZ(int z);
		int getZ();

	private:
		glm::vec2 m_points[8];
		glm::vec2 m_uvs[8];
		int m_num_points;
		ObjectType m_type;
		std::string m_name;
		float m_minx;
		float m_maxx;
		float m_miny;
		float m_maxy;
		int m_vbo_index;
		int m_id;
		int m_z;
		Param m_params[8];

		static Level* m_parent;

		void calculateBoundingBox();
	};

	struct VBO
	{
		glm::vec3 position;
		glm::vec2 uv;
	};

	struct Prefab
	{
		glm::vec4 points[8];
		int num_points;
		std::string name;
		int id;
		float anchor_leftx;
		float anchor_topy;
		float anchor_rightx;
		float anchor_bottomy;
	};

	Level();
	~Level();

	int insertObject(glm::vec2* points, glm::vec2* uvs, int num_points, ObjectType type, std::string name);
	void editObjectGeo(int object, glm::vec2* points, glm::vec2* uvs);
	void editObjectGeo(int object, glm::vec2* points);
	void editObjectUVs(int object, glm::vec2* uvs);
	void removeObject(int object);
	void removeObjectById(int id);
	void removeObjects();
	int numObjects();
	Level::Object* getObject(int object);
	Level::Object* getObjectById(int id);
	int getIndexById(int id);
	void reset();
	float* getVBO(int index);
	int numVBOVerts(int index);
	bool isModified();
	void resetModify();
	bool isVBOUpdated();
	void resetVBOUpdate();
	void setTriggerList(std::vector<std::string>& list);
	const std::string& getTriggerName(int index);
	int getNumTriggers();
	int getNumPrefabs();
	const Level::Prefab* getPrefab(int index);
	const Level::Prefab* getPrefabById(int id);
	int insertPrefab(std::string name, glm::vec4* points, int num_points);
	void removePrefab(int id);
	void removePrefabs();

	void resizeTilemap(int xstart, int xend, int ystart, int yend, float tile_size);
	int readTilemap(int x, int y);
	void editTilemap(int x, int y, int tile);	
	float* getTilemapVBO();
	int numTilemapTris();
	const Tilemap::Config& getTilemapConfig();
	int insertTile(std::string name, glm::vec2* points);
	void removeTile(int id);
	void removeTiles();
	int getNumTiles();
	const Tilemap::Tile& getTile(int index);
	const Tilemap::Tile* getTileById(int id);
	int getTileIndexById(int id);

private:
	int tesselateObject(int object);
	void tesselateAll();

	std::vector<Object*> m_objects;
	VBO* m_vbo[NUM_VBOS];
	int m_num_verts[NUM_VBOS];
	int m_cumulative_object_id;
	int m_cumulative_prefab_id;

	bool m_modified;

	bool m_vbo_updated;

	std::vector<std::string> m_trigger_list;
	std::vector<Level::Prefab> m_prefabs;

	Tilemap* m_tilemap;
};