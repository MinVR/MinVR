/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRRENDERSTATE_H_
#define VRRENDERSTATE_H_

#include "config/VRDataIndex.h"
#include "config/VRSerializable.h"

namespace MinVR {

class VRRenderState {
public:
	VRRenderState();
	virtual ~VRRenderState();

	VRDataIndex& getDataIndex();

	const std::string& getNameSpace() const {
		return m_nameSpace;
	}

	void setNameSpace(const std::string& nameSpace) {
		this->m_nameSpace = nameSpace;
	}

	bool deserializeValue(std::string name, VRSerializable& serializable)
	{
		return serializable.deserialize(m_index, m_nameSpace + "/" + name);
	}

	void serializeValue(std::string name, const VRSerializable& serializable)
	{
		serializable.serialize(m_index, m_nameSpace + "/" + name);
	}

	VRAnyCoreType getValue(std::string name)
	{
		return m_index.getValue(name, m_nameSpace);
	}

	template<typename T>
	T getValue(std::string name, T defaultValue)
	{
		if (!m_index.exists(name, m_nameSpace))
		{
			return defaultValue;
		}

		return getValue(name);
	}

	template<typename T>
	void setValue(std::string name, T val)
	{
		m_index.addData(m_nameSpace + "/" + name, val);
	}

private:
	VRDataIndex m_index;
	std::string m_nameSpace;
};

} /* namespace MinVR */

#endif /* VRRENDERSTATE_H_ */
