#include "Level.h"


TiledObject::TiledObject(int id, glm::vec2 position, int width, int height, float tile_size, const TiledObject::Definition& tiledef)
{
	m_id = id;

	m_position = position;
	m_width = width;
	m_height = height;
	m_tile_size = tile_size;

	m_z = 0;

	m_tiledef.width = tiledef.width;
	m_tiledef.height = tiledef.height;

	m_tiledef.middle = tiledef.middle;
	m_tiledef.edge_left = tiledef.edge_left;
	m_tiledef.edge_right = tiledef.edge_right;
	m_tiledef.edge_top = tiledef.edge_top;
	m_tiledef.edge_bottom = tiledef.edge_bottom;
	m_tiledef.corner_tl = tiledef.corner_tl;
	m_tiledef.corner_tr = tiledef.corner_tr;
	m_tiledef.corner_bl = tiledef.corner_bl;
	m_tiledef.corner_br = tiledef.corner_br;

	if (m_tiledef.edge_top < 0)
		m_tiledef.edge_top = m_tiledef.middle;
	if (m_tiledef.edge_bottom < 0)
		m_tiledef.edge_bottom = m_tiledef.middle;
	if (m_tiledef.edge_left < 0)
		m_tiledef.edge_left = m_tiledef.middle;
	if (m_tiledef.edge_right < 0)
		m_tiledef.edge_right = m_tiledef.middle;
	
	if (m_tiledef.corner_tl < 0)
		m_tiledef.corner_tl = m_tiledef.edge_top;
	if (m_tiledef.corner_tr < 0)
		m_tiledef.corner_tr = m_tiledef.edge_top;
	if (m_tiledef.corner_bl < 0)
		m_tiledef.corner_bl = m_tiledef.edge_bottom;
	if (m_tiledef.corner_br < 0)
		m_tiledef.corner_br = m_tiledef.edge_bottom;
}

TiledObject::~TiledObject()
{

}



// init static member
Level* Level::Object::m_parent = NULL;

Level::Object::Object(int id, glm::vec2* points, glm::vec2* uvs, int num_points, ObjectType type, std::string& name, unsigned int color)
{
	m_num_points = num_points;
	m_type = type;
	m_name = name;
	m_id = id;
	m_z = 0;

	m_color = color;

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
		m_parent->setModified();

		//TODO: full tesselate

		//int index = m_parent->getIndexById(this->m_id);
		//m_parent->tesselateObject(index);
		m_parent->tesselateAllObjects();
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
		m_parent->setModified();

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
		m_parent->setModified();

		// flag for screen update
		m_parent->m_vbo_updated = true;

		// retesselate
		int index = m_parent->getIndexById(this->m_id);
		m_parent->tesselateObject(index);
	}
}

unsigned int Level::Object::getColor()
{
	return m_color;
}

void Level::Object::setColor(unsigned int color)
{
	if (m_color != color)
	{
		m_color = color;

		// flag level modified
		m_parent->setModified();

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
	std::string::size_type pos = source.m_name.rfind("- Copy");
	if (pos != std::string::npos)
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


int Level::insertObject(glm::vec2* points, glm::vec2* uvs, int num_points, ObjectType type, std::string name, unsigned int color)
{
	int id = m_cumulative_object_id;
	m_cumulative_object_id++;

	Level::Object* obj = new Level::Object(id, points, uvs, num_points, type, name, color);

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

	setModified();

	return id;
}

void Level::removeObject(int object)
{
	assert(object >= 0 && object < (int)m_objects.size());

	Level::Object* obj = m_objects.at(object);
	delete obj;

	m_objects.erase(m_objects.begin() + object);

	setModified();

	tesselateAllObjects();
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

			setModified();
			tesselateAllObjects();
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

	setModified();
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

	setModified();
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
	
	setModified();
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

			m_vbo[vbo][index + 0].position = p0;		m_vbo[vbo][index + 0].uv = uv0;		m_vbo[vbo][index + 0].color = obj->m_color;
			m_vbo[vbo][index + 1].position = p1;		m_vbo[vbo][index + 1].uv = uv1;		m_vbo[vbo][index + 1].color = obj->m_color;
			m_vbo[vbo][index + 2].position = p2;		m_vbo[vbo][index + 2].uv = uv2;		m_vbo[vbo][index + 2].color = obj->m_color;

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

void Level::tesselateAllObjects()
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

	tesselateAllObjects();

	setModified();
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

	setModified();
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

void Level::setModified()
{
	m_modified = true;
}

bool Level::isVBOUpdated()
{
	return m_vbo_updated;
}

void Level::resetVBOUpdate()
{
	m_vbo_updated = false;
}

void Level::setTriggerList(std::vector<std::string>& list)
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

int Level::insertPrefab(std::string name, glm::vec4* points, int num_points, unsigned int color)
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

	prefab.color = color;

	m_prefabs.push_back(prefab);

	setModified();
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
			setModified();
			return;
		}
	}
}

void Level::removePrefabs()
{
	m_prefabs.clear();

	setModified();
}



void Level::resizeTilemap(int xstart, int xend, int ystart, int yend, float tile_size)
{
	m_tilemap->resize(xstart, ystart, xend, yend, tile_size);
	setModified();
}

int Level::readTilemap(int x, int y)
{
	return m_tilemap->get(x, y);
}

void Level::editTilemap(int x, int y, int tile)
{
	m_tilemap->edit(x, y, tile);
	setModified();
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


int Level::insertTile(std::string name, glm::vec2* points, unsigned int color)
{
	setModified();
	return m_tilemap->insertTile(name, points, color);
}

void Level::removeTile(int id)
{
	bool removed = m_tilemap->removeTile(id);
	if (removed)
		setModified();
}

void Level::removeTiles()
{
	m_tilemap->removeTiles();
	setModified();
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


int Level::insertTiledObject(const glm::vec2& position, int width, int height, float tile_size, const TiledObject::Definition& tiledef)
{
	int id = m_cumulative_tiledobject_id;
	m_cumulative_tiledobject_id++;

	TiledObject* obj = new TiledObject(id, position, width, height, tile_size, tiledef);
	m_tiledobjects.push_back(obj);

	m_tiledobjects.back()->m_vbo_index = m_num_verts[VBO_TILEDOBJECT];
	int size = tesselateTiledObject(m_tiledobjects.size() - 1);
	m_num_verts[VBO_TILEDOBJECT] += size;

	setModified();

	return id;
}

void Level::removeTiledObject(int object)
{
	assert(object >= 0 && object < (int)m_tiledobjects.size());

	TiledObject* obj = m_tiledobjects.at(object);
	delete obj;

	m_tiledobjects.erase(m_tiledobjects.begin() + object);

	setModified();

	tesselateAllTiledObjects();
}

void Level::removeTiledObjectById(int id)
{
	int num_objs = m_tiledobjects.size();
	for (int i = 0; i < num_objs; i++)
	{
		TiledObject* obj = m_tiledobjects.at(i);
		if (obj->m_id == id)
		{
			m_tiledobjects.erase(m_tiledobjects.begin() + i);
			delete obj;

			setModified();
			tesselateAllTiledObjects();
			return;
		}
	}
}

int Level::tesselateTiledObject(int object)
{
	TiledObject* obj = m_tiledobjects[object];
	int index = obj->m_vbo_index;

	float z = (float)(obj->m_z) + 100.0f;

	for (int j = 0; j < obj->m_height; j++)
	{
		for (int i = 0; i < obj->m_width; i++)
		{
			TiledObject::TileType tiletype;

			if (i == 0 && j == 0)
				tiletype = TiledObject::TILE_CORNER_TL;
			else if (i == obj->m_width - 1 && j == 0)
				tiletype = TiledObject::TILE_CORNER_TR;
			else if (i == 0 && j == obj->m_height - 1)
				tiletype = TiledObject::TILE_CORNER_BL;
			else if (i == obj->m_width - 1 && j == obj->m_height - 1)
				tiletype = TiledObject::TILE_CORNER_BR;
			else if (i == 0)
				tiletype = TiledObject::TILE_EDGE_LEFT;
			else if (i == obj->m_width - 1)
				tiletype = TiledObject::TILE_EDGE_RIGHT;
			else if (j == 0)
				tiletype = TiledObject::TILE_EDGE_TOP;
			else if (j == obj->m_height - 1)
				tiletype = TiledObject::TILE_EDGE_BOTTOM;
			else
				tiletype = TiledObject::TILE_MIDDLE;

			int tile_index = 0;

			switch (tiletype)
			{
				case TiledObject::TILE_CORNER_TL:	tile_index = obj->m_tiledef.corner_tl; break;
				case TiledObject::TILE_CORNER_TR:	tile_index = obj->m_tiledef.corner_tr; break;
				case TiledObject::TILE_CORNER_BL:	tile_index = obj->m_tiledef.corner_bl; break;
				case TiledObject::TILE_CORNER_BR:	tile_index = obj->m_tiledef.corner_br; break;
				case TiledObject::TILE_EDGE_TOP:	tile_index = obj->m_tiledef.edge_top; break;
				case TiledObject::TILE_EDGE_BOTTOM:	tile_index = obj->m_tiledef.edge_bottom; break;
				case TiledObject::TILE_EDGE_LEFT:	tile_index = obj->m_tiledef.edge_left; break;
				case TiledObject::TILE_EDGE_RIGHT:	tile_index = obj->m_tiledef.edge_right; break;
				default:							tile_index = obj->m_tiledef.middle; break;
			}

			float w = obj->m_tile_size * obj->m_tiledef.width;
			float h = obj->m_tile_size * obj->m_tiledef.height;

			float sx = obj->m_position.x + (i * w);
			float ex = sx + w;
			float sy = obj->m_position.y + (j * w);
			float ey = sy + h;

			glm::vec3 p0 = glm::vec3(sx, sy, z);
			glm::vec3 p1 = glm::vec3(sx, ey, z);
			glm::vec3 p2 = glm::vec3(ex, ey, z);
			glm::vec3 p3 = glm::vec3(ex, sy, z);

			glm::vec2 uv0, uv1, uv2, uv3;

			if (m_tilemap->getNumTiles() > 0)
			{
				const Tilemap::Tile& tile = m_tilemap->getTile(tile_index);

				uv0 = tile.points[0];
				uv1 = tile.points[1];
				uv2 = tile.points[2];
				uv3 = tile.points[3];
			}
			else
			{
				uv0 = glm::vec2(0.0f, 0.0f);
				uv1 = glm::vec2(0.0f, 1.0f);
				uv2 = glm::vec2(1.0f, 1.0f);
				uv3 = glm::vec2(1.0f, 0.0f);
			}

			m_vbo[VBO_TILEDOBJECT][index + 0].position = p0;	m_vbo[VBO_TILEDOBJECT][index + 0].uv = uv0;
			m_vbo[VBO_TILEDOBJECT][index + 1].position = p1;	m_vbo[VBO_TILEDOBJECT][index + 1].uv = uv1;
			m_vbo[VBO_TILEDOBJECT][index + 2].position = p2;	m_vbo[VBO_TILEDOBJECT][index + 2].uv = uv2;
			m_vbo[VBO_TILEDOBJECT][index + 3].position = p0;	m_vbo[VBO_TILEDOBJECT][index + 3].uv = uv0;
			m_vbo[VBO_TILEDOBJECT][index + 4].position = p2;	m_vbo[VBO_TILEDOBJECT][index + 4].uv = uv2;
			m_vbo[VBO_TILEDOBJECT][index + 5].position = p3;	m_vbo[VBO_TILEDOBJECT][index + 5].uv = uv3;

			index += 6;
		}
	}

	m_vbo_updated = true;

	return index - obj->m_vbo_index;
}

void Level::tesselateAllTiledObjects()
{
	int num_objs = m_tiledobjects.size();

	for (int i = 0; i < num_objs; i++)
	{
		TiledObject* obj = m_tiledobjects[i];

		obj->m_vbo_index = m_num_verts[VBO_TILEDOBJECT];
		m_num_verts[VBO_TILEDOBJECT] += tesselateTiledObject(i);
	}
}

void Level::removeTiledObjects()
{
	int num_objs = m_tiledobjects.size();

	for (int i = 0; i < num_objs; i++)
	{
		delete m_tiledobjects[i];
	}

	m_tiledobjects.clear();

	tesselateAllTiledObjects();

	setModified();
}

int Level::numTiledObjects()
{
	return (int)m_tiledobjects.size();
}