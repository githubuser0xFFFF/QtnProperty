#include "VarProperty.h"

#include "QtnProperty/Core/PropertyBase.h"
#include "QtnProperty/Core/PropertySet.h"
#include "QtnProperty/Core/Core/PropertyBool.h"
#include "QtnProperty/Core/Core/PropertyInt.h"
#include "QtnProperty/Core/Core/PropertyUInt.h"
#include "QtnProperty/Core/Core/PropertyDouble.h"
#include "QtnProperty/Core/Core/PropertyQString.h"

VarProperty::VarProperty(QObject *parent, VarProperty::Type type, const QString &name, int index, const QVariant &value)
	: QObject(parent)
	, var_parent(nullptr)
	, value(type == Value ? value : QVariant())
	, name(name)
	, index(index)
	, type(type)
{

}

void VarProperty::ChangePropertyValue(QVariant &dest, const QVariant &value)
{
	if (this->value != value)
	{
		this->value = value;

		auto top = TopParent();

		Q_ASSERT(nullptr != top);

		dest = top->CreateVariant();
	}
}

void VarProperty::RemoveFromParent()
{
	auto &siblings = var_parent->var_children;

	auto it = std::find_if(siblings.begin(), siblings.end(), [this](VarProperty *value)->bool
	{
		return value == this;
	});

	if (it != siblings.end())
	{
		siblings.erase(it);
	}
}

VarProperty *VarProperty::Duplicate(VarProperty *child, int new_index)
{
	Q_ASSERT(nullptr != child);

	return AddChild(new VarProperty(nullptr, VarProperty::Value, "", new_index,
									child->CreateVariant()), new_index);
}

VarProperty *VarProperty::Duplicate(VarProperty *child, const QString &new_name)
{
	Q_ASSERT(nullptr != child);

	return AddChild(new VarProperty(nullptr, VarProperty::Value, new_name,
									-1, child->CreateVariant()));
}

VarProperty *VarProperty::Duplicate(int new_index)
{
	Q_ASSERT(nullptr != var_parent);
	return var_parent->Duplicate(this, new_index);
}

VarProperty *VarProperty::Duplicate(const QString &new_name)
{
	Q_ASSERT(nullptr != var_parent);
	return var_parent->Duplicate(this, new_name);
}

VarProperty *VarProperty::AddChild(VarProperty *child, int index)
{
	if (index < 0)
		index = var_children.size();

	child->var_parent = this;
	var_children.insert(var_children.begin() + index, child);

	return child;
}

VarProperty *VarProperty::AddChild(int index, const QVariant &value)
{
	return AddChild(new VarProperty(nullptr, GetTypeFromValue(value), "", index, value), index);
}

VarProperty *VarProperty::AddChild(const QString &name, const QVariant &value)
{
	return AddChild(new VarProperty(nullptr, GetTypeFromValue(value), name, -1, value));
}

VarProperty::Type VarProperty::GetTypeFromValue(const QVariant &value)
{
	Type type;
	switch (value.type())
	{
		case QVariant::Map:
			type = Map;
			break;

		case QVariant::List:
			type = List;
			break;

		default:
			type = Value;
			break;
	}

	return type;
}

VarProperty::Type VarProperty::GetType() const
{
	return type;
}

int VarProperty::GetIndex() const
{
	return index;
}

void VarProperty::SetIndex(int new_index)
{
	index = new_index;
}

const QString &VarProperty::GetName() const
{
	return name;
}

void VarProperty::SetName(const QString &new_name)
{
	name = new_name;
}

VarProperty *VarProperty::TopParent()
{
	auto p = var_parent;
	if (nullptr == p)
		return this;

	while (nullptr != p)
	{
		auto next_p = p->var_parent;
		if (nullptr == next_p)
			return p;

		p = next_p;
	}

	return nullptr;
}

VarProperty *VarProperty::VarParent()
{
	return var_parent;
}

VarProperty::VarChildren &VarProperty::GetVarChildren()
{
	return var_children;
}

QVariant VarProperty::CreateVariant() const
{
	switch (type)
	{
		case List:
		{
			QVariantList list;

			for (auto child : var_children)
			{
				list.append(child->CreateVariant());
			}

			return QVariant(list);
		}

		case Map:
		{
			QVariantMap map;

			for (auto child : var_children)
			{
				map.insert(child->name, child->CreateVariant());
			}

			return QVariant(map);
		}

		default:
			break;
	}

	return value;
}

bool VarProperty::IsChildNameAvailable(const QString &name, bool skip_this) const
{
	auto it = std::find_if(var_children.begin(), var_children.end(),
	[this, &name](VarProperty *value) -> bool
	{
		if (skip_this && value == this)
			return false;

		return value->name == name;
	});

	return it == var_children.end();
}

QtnPropertyBase *VarProperty::NewExtraProperty(QtnPropertySet *set, const QVariant &value,
											   const QString &key, int index, VarProperty *map_parent,
											   const RegisterPropertyCallback &register_property)
{
	QtnProperty *prop = nullptr;

	auto type = value.type();

	switch (type)
	{
		case QVariant::Map:
		{
			return NewExtraPropertySet(set, value.toMap(), map_parent, key, index, register_property);
		} break;

		case QVariant::List:
		{
			return NewExtraPropertyList(set, value.toList(), map_parent, key, index, register_property);
		} break;

		case QVariant::String:
		case QVariant::Char:
		{
			auto p = new QtnPropertyQString(set);

			p->setId(PID_EXTRA_STRING);
			p->setValue(value.toString());

			prop = p;
		} break;

		case QVariant::Int:
		{
			auto p = new QtnPropertyInt(set);

			p->setId(PID_EXTRA_INT);
			p->setValue(value.toInt());

			prop = p;
		} break;

		case QVariant::UInt:
		{
			auto p = new QtnPropertyUInt(set);

			p->setId(PID_EXTRA_UINT);
			p->setValue(value.toUInt());

			prop = p;
		} break;

		case QVariant::LongLong:
		case QVariant::ULongLong:
		case QVariant::Double:
		{
			auto p = new QtnPropertyDouble(set);

			p->setId(PID_EXTRA_FLOAT);
			p->setValue(value.toDouble());

			prop = p;
		} break;

		case QVariant::Bool:
		{
			auto p = new QtnPropertyBool(set);

			p->setId(PID_EXTRA_BOOL);
			p->setValue(value.toBool());

			prop = p;
		} break;

		default:
			break;
	}

	if (nullptr != prop)
	{
		auto varprop = new VarProperty(prop, VarProperty::Value, key, index, value);
		if (nullptr != map_parent)
			map_parent->AddChild(varprop);
		prop->setName(key);
		register_property(prop);
	}

	return prop;
}

bool VarProperty::PropertyValueAccept(const QtnProperty *property, void *valueToAccept, QVariant &dest)
{
	switch (property->id())
	{
		case VarProperty::PID_EXTRA:
		case VarProperty::PID_EXTRA_STRING:
		case VarProperty::PID_EXTRA_INT:
		case VarProperty::PID_EXTRA_UINT:
		case VarProperty::PID_EXTRA_FLOAT:
		case VarProperty::PID_EXTRA_BOOL:
		{
			auto var_property = property->findChild<VarProperty *>(QString(), Qt::FindDirectChildrenOnly);

			QVariant value;

			if (nullptr != valueToAccept)
			{
				switch (property->id())
				{
					case PID_EXTRA_STRING:
						value = QVariant(*(QtnPropertyQString::ValueType *) valueToAccept);
						break;

					case PID_EXTRA_INT:
						value = QVariant(*(QtnPropertyInt::ValueType *) valueToAccept);
						break;

					case PID_EXTRA_UINT:
						value = QVariant(*(QtnPropertyUInt::ValueType *) valueToAccept);
						break;

					case PID_EXTRA_FLOAT:
						value = QVariant(*(QtnPropertyDouble::ValueType *) valueToAccept);
						break;

					case PID_EXTRA_BOOL:
						value = QVariant(*(QtnPropertyBool::ValueType *) valueToAccept);
						break;

					default:
						value = *(QVariant *) valueToAccept;
						break;
				}
			}

			var_property->ChangePropertyValue(dest, value);

			return true;
		}
	}

	return false;
}

QtnPropertySet *VarProperty::NewExtraPropertySet(QObject *parent, const QVariantMap &map,
												 VarProperty *map_parent, const QString &name,
												 int index, const RegisterPropertyCallback &register_property)
{
	auto set = new QtnPropertySet(parent);

	auto varprop = new VarProperty(set, VarProperty::Map, name, index, QVariant());
	if (nullptr != map_parent)
		map_parent->AddChild(varprop);
	map_parent = varprop;

	set->setId(PID_EXTRA);
	set->setName(name);

	for (auto it = map.begin(); it != map.end(); ++it)
	{
		auto &key = it.key();
		auto &value = it.value();

		NewExtraProperty(set, value, key, -1, map_parent, register_property);
	}

	return set;
}

QtnPropertySet *VarProperty::NewExtraPropertyList(QObject *parent, const QVariantList &list,
												  VarProperty *map_parent, const QString &name,
												  int index, const RegisterPropertyCallback &register_property)
{
	auto set = new QtnPropertySet(parent);

	auto varprop = new VarProperty(set, VarProperty::List, name, index, QVariant());
	if (nullptr != map_parent)
		map_parent->AddChild(varprop);
	map_parent = varprop;

	set->setId(PID_EXTRA);
	set->setName(name);

	int len = list.size();

	for (int i = 0; i < len; i++)
	{
		auto &value = list.at(i);

		NewExtraProperty(set, value, QString("[%1]").arg(QString::number(i)), i, map_parent, register_property);
	}

	return set;
}