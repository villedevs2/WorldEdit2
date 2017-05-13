#include "Level.h"

// init static member
Level* Level::Object::m_parent = NULL;

Level::Object::Object(int id, glm::vec2* points, glm::vec2* uvs, int num_points, ObjectType type, std::string& name)
{
	m_num_points = num_points;
	m_type = type;
	m_name = name;
	m_id = id;
	m_z = 0;

	for (int i=0; i < num_points; i++)
	{
		m_points[i] = points[i];
		m_uvs[i] = uvs[i];
	}

	for (int i=0; i < num_points; i++)
	{
		m_params[i].i = 0;
	}
}

Level::Object::~Object()
{
}

void Level::Object::setParent(Level* level)
{
	m_parent = level;
}

bool Level::Object::pointInside(glm::vec2 point, float threshold)
{
	// quick bounding box check
	if (point.x < (m_minx - threshold) ||
		point.x > (m_maxx + threshold) ||
		point.y < (m_miny - threshold) ||
		point.y > (m_maxy + threshold))
		return false;

	for (int i=0; i < m_num_points; i++)
	{
		glm::vec2 p0, p1;
		if (i == (m_num_points - 1))
		{
			p0 = m_points[i];
			p1 = m_points[0];
		}
		else
		{
			p0 = m_points[i];
			p1 = m_points[i+1];
		}

		
		glm::vec2 edge = p1 - p0;
		glm::vec2 norm = glm::vec2(-edge.y, edge.x);
		glm::vec2 vecb = p1 - point;

		float dot = glm::dot(norm, vecb);
		if (dot < (0.0f))
			return false;
		

		// TODO: thresholded select is buggy...

		// nudge the edge forward for threshold
		/*
		glm::vec2 edge = p1 - p0;
		glm::vec2 norm = glm::normalize(glm::vec2(-edge.y, edge.x)) * threshold;
		p0 += norm;
		p1 += norm;

		edge = p1 - p0;
		norm = glm::vec2(-edge.y, edge.x);
		glm::vec2 vecb = p1 - point;
		

		float dot = glm::dot(norm, vecb);
		if (dot < (0.0f + threshold))
			return false;
		*/
	}

	return true;
}

void Level::Object::calculateBoundingBox()
{
	m_minx = m_points[0].x;
	m_maxx = m_minx;
	m_miny = m_points[0].y;
	m_maxy = m_miny;

	for (int i=1; i < m_num_points; i++)
	{
		if (m_points[i].x < m_minx)
			m_minx = m_points[i].x;
		if (m_points[i].x > m_maxx)
			m_maxx = m_points[i].x;
		if (m_points[i].y < m_miny)
			m_miny = m_points[i].y;
		if (m_points[i].y > m_maxy)
			m_maxy = m_points[i].y;
	}
}

glm::vec2 Level::Object::getBoundingMin()
{
	return glm::vec2(m_minx, m_miny);
}

glm::vec2 Level::Object::getBoundingMax()
{
	return glm::vec2(m_maxx, m_maxy);
}

std::string& Level::Object::getName()
{
	return m_name;
}

void Level::Object::setName(std::string& name)
{
	m_name = name;

	// flag level modified
	m_parent->m_modified = true;
}

Level::ObjectType Level::Object::getType()
{
	return m_type;
}

void Level::Object::setType(Level::ObjectType type)
{
	if (m_type != type)
	{
		m_type = type;

		// flag level modified
		m_parent->m_modified = true;

		//TODO: full tesselate

		//int index = m_parent->getIndexById(this->m_id);
		//m_parent->tesselateObject(index);
		m_parent->tesselateAll();
	}
}


const glm::vec2& Level::Object::getPoint(int i)
{
	assert(i >= 0 && i < m_num_points);
	return m_points[i];
}

const glm::vec2& Level::Object::getUV(int i)
{
	assert(i >= 0 && i < m_num_points);
	return m_uvs[i];
}

void Level::Object::insertPoint(glm::vec2 pos, glm::vec2 uv)
{
	assert(m_num_points < 8);
	m_points[m_num_points] = pos;
	m_uvs[m_num_points] = uv;
	m_num_points++;
}

int Level::Object::getNumPoints()
{
	return m_num_points;
}

int Level::Object::getId()
{
	return m_id;
}

void Level::Object::reset()
{
	m_num_points = 0;
}

void Level::Object::setParam(int index, Level::Object::Param data)
{
	assert(index >= 0 && index < Level::Object::NUM_PARAMS);

	if (m_params[index].i != data.i)
	{
		m_params[index] = data;

		// flag level modified
		m_parent->m_modified = true;

		// flag for screen update
		m_parent->m_vbo_updated = true;
	}
}

Level::Object::Param Level::Object::getParam(int index)
{
	assert(index >= 0 && index < Level::Object::NUM_PARAMS);

	return m_params[index];
}

int Level::Object::getZ()
{
	return m_z;
}

void Level::Object::setZ(int z)
{
	if (m_z != z)
	{
		m_z = z;

		// flag level modified
		m_parent->m_modified = true;

		// flag for screen update
		m_parent->m_vbo_updated = true;

		// retesselate
		int index = m_parent->getIndexById(this->m_id);
		m_parent->tesselateObject(index);
	}
}

void Level::Object::copy(const Level::Object& source)
{
	reset();

	for (int i=0; i < source.m_num_points; i++)
	{
		glm::vec2 pos = source.m_points[i];
		glm::vec2 uv = source.m_uvs[i];

		insertPoint(pos, uv);
	}

	// set name
	string::size_type pos = source.m_name.rfind("- Copy");
	if (pos != string::npos)
	{
		std::string numstr = source.m_name.substr(pos+6);
		int num = 1;
		if (numstr.length() > 1)
		{
			sscanf(numstr.c_str(), "%d", &num);
			num++;
		}

		char tempstr[20];
		sprintf(tempstr, "%d", num);

		m_name = source.m_name.substr(0, pos + 6) + " " + std::string(tempstr);
	}
	else
	{
		m_name = source.m_name + " - Copy";
	}

	m_type = source.m_type;

	m_z = source.m_z;

	for (int i=0; i < Level::Object::NUM_PARAMS; i++)
	{
		m_params[i] = source.m_params[i];
	}
}




Level::Level()
{
	m_tilemap = new Tilemap();

	for (int i=0; i < NUM_VBOS; i++)
	{
		m_vbo[i] = new VBO[65536];
	}
	reset();

	Level::Object::setParent(this);
}

Level::~Level()
{
	delete m_tilemap;

	for (int i=0; i < NUM_VBOS; i++)
	{
		delete [] m_vbo[i];
	}
}


int Level::insertObject(glm::vec2* points, glm::vec2* uvs, int num_points, ObjectType type, std::string name)
{
	int id = m_cumulative_object_id;
	m_cumulative_object_id++;

	Level::Object* obj = new Level::Object(id, points, uvs, num_points, type, name);

	m_objects.push_back(obj);

	m_objects.back()->calculateBoundingBox();

	int vbo_index = 0;
	switch (type)
	{
		case OBJECT_TYPE_COLLISION:		vbo_index = VBO_COLLISION; break;
		case OBJECT_TYPE_GEOVIS:		vbo_index = VBO_GEOVIS; break;
		case OBJECT_TYPE_TRIGGER:		vbo_index = -1; break;
		case OBJECT_TYPE_SLIDER:		vbo_index = VBO_SLIDER; break;
		case OBJECT_TYPE_DESTRUCTIBLE:	vbo_index = VBO_DESTRUCTIBLE; break;
		case OBJECT_TYPE_MOVER:			vbo_index = VBO_MOVER; break;
	}

	if (vbo_index >= 0)
	{
		m_objects.back()->m_vbo_index = m_num_verts[vbo_index];

		int size = tesselateObject(m_objects.size() - 1);
		m_num_verts[vbo_index] += size;
	}

	m_modified = true;

	return id;
}

void Level::removeObject(int object)
{
	assert(object >= 0 && object < (int)m_objects.size());

	Level::Object* obj = m_objects.at(object);
	delete obj;

	m_objects.erase(m_objects.begin() + object);

	m_modified = true;

	tesselateAll();
}

void Level::removeObjectById(int id)
{
	int num_objs = m_objects.size();
	for (int i=0; i < num_objs; i++)
	{
		Level::Object* obj = m_objects.at(i);
		if (obj->m_id == id)
		{
			m_objects.erase(m_objects.begin() + i);
			delete obj;

			m_modified = true;
			return;
		}
	}
}

void Level::editObjectGeo(int object, glm::vec2* points, glm::vec2* uvs)
{
	assert(object >= 0 && object < (int)m_objects.size());

	Level::Object* obj = m_objects[object];

	for (int i=0; i < obj->m_num_points; i++)
	{
		obj->m_points[i] = points[i];
		obj->m_uvs[i] = uvs[i];
	}

	obj->calculateBoundingBox();
	tesselateObject(object);

	m_modified = true;
}

void Level::editObjectGeo(int object, glm::vec2* points)
{
	assert(object >= 0 && object < (int)m_objects.size());

	Level::Object* obj = m_objects[object];

	for (int i=0; i < obj->m_num_points; i++)
	{
		obj->m_points[i] = points[i];
	}

	obj->calculateBoundingBox();
	tesselateObject(object);

	m_modified = true;
}

void Level::editObjectUVs(int object, glm::vec2* uvs)
{
	assert(object >= 0 && object < (int)m_objects.size());

	Level::Object* obj = m_objects[object];

	for (int i=0; i < obj->m_num_points; i++)
	{
		obj->m_uvs[i] = uvs[i];
	}

	tesselateObject(object);
	
	m_modified = true;
}

int Level::tesselateObject(int object)
{
	Level::Object* obj = m_objects[object];
	int index = obj->m_vbo_index;

	float z = (float)(obj->m_z) + 100.0f;
	
	Level::ObjectType type = obj->getType();

	if (type == OBJECT_TYPE_COLLISION)
		z = 1.0f;

	int vbo = -1;
	switch (type)
	{
		case OBJECT_TYPE_COLLISION:		vbo = VBO_COLLISION; break;
		case OBJECT_TYPE_GEOVIS:		vbo = VBO_GEOVIS; break;
		case OBJECT_TYPE_TRIGGER:		vbo = -1; break;
		case OBJECT_TYPE_SLIDER:		vbo = VBO_SLIDER; break;
		case OBJECT_TYPE_DESTRUCTIBLE:	vbo = VBO_DESTRUCTIBLE; break;
		case OBJECT_TYPE_MOVER:			vbo = VBO_MOVER; break;
	}

	if (vbo >= 0)
	{
		for (int poly=2; poly < obj->m_num_points; poly++)
		{
			glm::vec3 p0 = glm::vec3(obj->m_points[0], z);
			glm::vec3 p1 = glm::vec3(obj->m_points[poly-1], z);
			glm::vec3 p2 = glm::vec3(obj->m_points[poly], z);

			glm::vec2 uv0 = obj->m_uvs[0];
			glm::vec2 uv1 = obj->m_uvs[poly-1];
			glm::vec2 uv2 = obj->m_uvs[poly];

			m_vbo[vbo][index+0].position = p0;		m_vbo[vbo][index+0].uv = uv0;
			m_vbo[vbo][index+1].position = p1;		m_vbo[vbo][index+1].uv = uv1;
			m_vbo[vbo][index+2].position = p2;		m_vbo[vbo][index+2].uv = uv2;

			index += 3;
		}
	}

	/*
	if (type == OBJECT_TYPE_GEO ||
		type == OBJECT_TYPE_DECO ||
		type == OBJECT_TYPE_SLIDER ||
		type == OBJECT_TYPE_DESTRUCTIBLE ||
		type == OBJECT_TYPE_MOVER)
	{
		for (int poly=2; poly < obj->m_num_points; poly++)
		{
			glm::vec3 p0 = glm::vec3(obj->m_points[0], z);
			glm::vec3 p1 = glm::vec3(obj->m_points[poly-1], z);
			glm::vec3 p2 = glm::vec3(obj->m_points[poly], z);

			glm::vec2 uv0 = obj->m_uvs[0];
			glm::vec2 uv1 = obj->m_uvs[poly-1];
			glm::vec2 uv2 = obj->m_uvs[poly];

			m_vbo[index+0].position = p0;		m_vbo[index+0].uv = uv0;
			m_vbo[index+1].position = p1;		m_vbo[index+1].uv = uv1;
			m_vbo[index+2].position = p2;		m_vbo[index+2].uv = uv2;

			index += 3;
		}
	}
	else		// make degenerates for non-rendered objects
	{
		for (int poly=2; poly < obj->m_num_points; poly++)
		{
			m_vbo[index+0].position = glm::vec3(0.0f, 0.0f, 0.0f);		m_vbo[index+0].uv = glm::vec2(0.0f, 0.0f);
			m_vbo[index+1].position = glm::vec3(0.0f, 0.0f, 0.0f);		m_vbo[index+0].uv = glm::vec2(0.0f, 0.0f);
			m_vbo[index+2].position = glm::vec3(0.0f, 0.0f, 0.0f);		m_vbo[index+0].uv = glm::vec2(0.0f, 0.0f);
			index += 3;
		}
	}
	*/

	m_vbo_updated = true;

	return index - obj->m_vbo_index;
}

void Level::tesselateAll()
{
	for (int i=0; i < NUM_VBOS; i++)
	{
		m_num_verts[i] = 0;
	}

	int num_objs = m_objects.size();

	for (int i=0; i < num_objs; i++)
	{
		Level::Object* obj = m_objects[i];

		int vbo = -1;
		switch (obj->getType())
		{
			case OBJECT_TYPE_COLLISION:		vbo = VBO_COLLISION; break;
			case OBJECT_TYPE_GEOVIS:		vbo = VBO_GEOVIS; break;
			case OBJECT_TYPE_TRIGGER:		vbo = -1; break;
			case OBJECT_TYPE_SLIDER:		vbo = VBO_SLIDER; break;
			case OBJECT_TYPE_DESTRUCTIBLE:	vbo = VBO_DESTRUCTIBLE; break;
			case OBJECT_TYPE_MOVER:			vbo = VBO_MOVER; break;
		}

		if (vbo >= 0)
		{
			obj->m_vbo_index = m_num_verts[vbo];

			m_num_verts[vbo] += tesselateObject(i);
		}
	}
}

void Level::removeObjects()
{
	int num_objects = m_objects.size();

	for (int i=0; i < num_objects; i++)
	{
		delete m_objects[i];
	}

	m_objects.clear();

	tesselateAll();

	m_modified = true;
}

int Level::numObjects()
{
	return (int)m_objects.size();
}

Level::Object* Level::getObject(int object)
{
	assert(object >= 0 && object < (int)m_objects.size());

	return m_objects[object];
}

Level::Object* Level::getObjectById(int id)
{
	int num_objs = m_objects.size();
	for (int i=0; i < num_objs; i++)
	{
		Level::Object* obj = m_objects.at(i);
		if (obj->m_id == id)
		{
			return obj;
		}
	}
	return NULL;
}

int Level::getIndexById(int id)
{
	int num_objs = m_objects.size();
	for (int i=0; i < num_objs; i++)
	{
		Level::Object* obj = m_objects.at(i);
		if (obj->m_id == id)
		{
			return i;
		}
	}
	return -1;
}

void Level::reset()
{
	removeObjects();
	removePrefabs();

	for (int i=0; i < NUM_VBOS; i++)
	{
		m_num_verts[i] = 0;
	}
	m_cumulative_object_id = 1;
	m_cumulative_prefab_id = 1;

	m_tilemap->reset();

	m_modified = true;
}

float* Level::getVBO(int index)
{
	assert(index >= 0 && index < NUM_VBOS);
	return (float*)m_vbo[index];
}

int Level::numVBOVerts(int index)
{
	assert(index >= 0 && index < NUM_VBOS);
	return m_num_verts[index];
}

bool Level::isModified()
{
	return m_modified;
}

void Level::resetModify()
{
	m_modified = false;
}

bool Level::isVBOUpdated()
{
	return m_vbo_updated;
}

void Level::resetVBOUpdate()
{
	m_vbo_updated = false;
}

void Level::setTriggerList(vector<std::string>& list)
{
	int num = list.size();

	m_trigger_list.clear();
	for (int i=0; i < num; i++)
	{
		m_trigger_list.push_back(list.at(i));
	}
}

const std::string& Level::getTriggerName(int index)
{
	assert(index >= 0 && index < m_trigger_list.size());

	return m_trigger_list.at(index);
}

int Level::getNumTriggers()
{
	return (int)m_trigger_list.size();
}

int Level::getNumPrefabs()
{
	return m_prefabs.size();
}

const Level::Prefab* Level::getPrefab(int index)
{
	assert(index >= 0 && index < m_prefabs.size());

	return &m_prefabs.at(index);
}

const Level::Prefab* Level::getPrefabById(int id)
{
	int num_prefabs = m_prefabs.size();
	for (int i=0; i < num_prefabs; i++)
	{
		const Level::Prefab* prefab = &m_prefabs.at(i);
		if (prefab->id == id)
		{
			return prefab;
		}
	}
	return NULL;
}

int Level::insertPrefab(std::string name, glm::vec4* points, int num_points)
{
	assert(num_points >= 0 && num_points <= 8);

	float minx = points[0].x;
	float miny = points[0].y;
	float maxx = points[0].x;
	float maxy = points[0].y;

	for (int i=0; i < num_points; i++)
	{
		if (points[i].x < minx)
			minx = points[i].x;
		if (points[i].y < miny)
			miny = points[i].y;
		if (points[i].y > maxx)
			maxx = points[i].x;
		if (points[i].y > maxy)
			maxy = points[i].y;
	}
	
	Level::Prefab prefab;
	for (int i=0; i < num_points; i++)
	{
		prefab.points[i] = points[i];
	}
	prefab.num_points = num_points;
	prefab.name = name;
	prefab.anchor_leftx = minx;
	prefab.anchor_topy = miny;
	prefab.anchor_rightx = maxx;
	prefab.anchor_bottomy = maxy;
	prefab.id = m_cumulative_prefab_id;
	m_cumulative_prefab_id++;

	m_prefabs.push_back(prefab);

	m_modified = true;
	return prefab.id;
}

void Level::removePrefab(int id)
{
	int num_prefabs = m_prefabs.size();
	for (int i=0; i < num_prefabs; i++)
	{
		Level::Prefab* prefab = &m_prefabs.at(i);
		if (prefab->id == id)
		{
			m_prefabs.erase(m_prefabs.begin() + i);
			m_modified = true;
			return;
		}
	}
}

void Level::removePrefabs()
{
	m_prefabs.clear();

	m_modified = true;
}



void Level::resizeTilemap(int xstart, int xend, int ystart, int yend, float tile_size)
{
	m_tilemap->resize(xstart, ystart, xend, yend, tile_size);
	m_modified = true;
}

int Level::readTilemap(int x, int y)
{
	return m_tilemap->get(x, y);
}

void Level::editTilemap(int x, int y, int tile)
{
	m_tilemap->edit(x, y, tile);
	m_modified = true;
}

float* Level::getTilemapVBO()
{
	return m_tilemap->getVBO();
}

int Level::numTilemapTris()
{
	return m_tilemap->numTris();
}

const Tilemap::Config& Level::getTilemapConfig()
{
	return m_tilemap->getConfig();
}


int Level::insertTile(std::string name, glm::vec2* points)
{
	m_modified = true;
	return m_tilemap->insertTile(name, points);
}

void Level::removeTile(int id)
{
	bool removed = m_tilemap->removeTile(id);
	if (removed)
		m_modified = true;
}

void Level::removeTiles()
{
	m_tilemap->removeTiles();
	m_modified = true;
}

int Level::getNumTiles()
{
	return m_tilemap->getNumTiles();
}

const Tilemap::Tile& Level::getTile(int index)
{
	return m_tilemap->getTile(index);
}

const Tilemap::Tile* Level::getTileById(int id)
{
	return m_tilemap->getTileById(id);
}

int Level::getTileIndexById(int id)
{
	return m_tilemap->getTileIndexById(id);
}