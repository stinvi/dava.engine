#pragma once

#include "REPlatform/DataNodes/Selectable.h"

namespace DAVA
{
class SelectableGroup
{
public:
    using CollectionType = Vector<Selectable>;

public:
    bool operator==(const SelectableGroup& other) const;
    bool operator!=(const SelectableGroup& other) const;

    bool ContainsObject(const Any& object) const;

    const CollectionType& GetContent() const;
    CollectionType& GetMutableContent();

    bool IsEmpty() const;
    uint32 GetSize() const;
    void Clear();

    void Add(const CollectionType& newSelection);

    // @{ This three methods have approximately n^2 * log(n) complexety. So if you have some code like this
    //  for (Object obj: objects) selectableGroup.Add(obj);
    // you will have approximately n^3*log(n) complexety. Much beter way is create Vector<CollectionType> and call
    // SelectableGroup::Add(const CollectionType& newSelection)
    void Add(const Any& object);
    void Add(const Any& object, const AABBox3& box);
    void Remove(const Any& object);
    // @}

    void Exclude(const SelectableGroup&);
    void Join(const SelectableGroup&);

    template <typename F>
    void RemoveIf(F func);

    template <typename T>
    bool ContainsObjectsOfType() const;

    template <typename T>
    bool ContainsOnlyObjectsOfType() const;

    /*
	 * TODO : hide this function, recalculate box when needed
	 */
    void RebuildIntegralBoundingBox();
    const AABBox3& GetIntegralBoundingBox() const;

    bool SupportsTransformType(Selectable::TransformType transformType) const;
    Vector3 GetCommonWorldSpaceTranslationVector() const;

    const Selectable& GetFirst() const;

    void Lock();
    void Unlock();
    bool IsLocked() const;

    void RemoveObjectsWithDependantTransform();

    AABBox3 GetTransformedBoundingBox() const;

public:
    template <typename T>
    class Enumerator
    {
    public:
        class Iterator
        {
        public:
            Iterator(SelectableGroup::CollectionType&);
            Iterator(SelectableGroup::CollectionType&, size_type);
            void SetIndex(uint32);

            Iterator& operator++();
            bool operator!=(const Iterator&) const;

            T* operator*();

        private:
            size_type index = 0;
            size_type endIndex = 0;
            SelectableGroup::CollectionType& collection;
        };

    public:
        Enumerator(SelectableGroup* group, SelectableGroup::CollectionType& collection);
        ~Enumerator();

        Iterator& begin();
        Iterator& end();

    private:
        SelectableGroup* group = nullptr;
        Iterator iBegin;
        Iterator iEnd;
    };

    template <typename T>
    class ConstEnumerator
    {
    public:
        class Iterator
        {
        public:
            Iterator(const SelectableGroup::CollectionType&);
            Iterator(const SelectableGroup::CollectionType&, size_type);
            void SetIndex(uint32);

            Iterator& operator++();
            bool operator!=(const Iterator&) const;
            T* operator*() const;

        private:
            size_type index = 0;
            size_type endIndex = 0;
            const SelectableGroup::CollectionType& collection;
        };

    public:
        ConstEnumerator(const SelectableGroup* group, const SelectableGroup::CollectionType& collection);
        ~ConstEnumerator();

        const Iterator& begin() const;
        const Iterator& end() const;

    private:
        const SelectableGroup* group = nullptr;
        Iterator iBegin;
        Iterator iEnd;
    };

    template <typename T>
    Enumerator<T> ObjectsOfType();

    template <typename T>
    ConstEnumerator<T> ObjectsOfType() const;

private:
    void SortObjects();

    CollectionType objects;
    AABBox3 integralBoundingBox;
    uint32 lockCounter = 0;
};

template <typename F>
inline void SelectableGroup::RemoveIf(F func)
{
    DVASSERT(!IsLocked());
    objects.erase(std::remove_if(objects.begin(), objects.end(), func), objects.end());
}

inline const SelectableGroup::CollectionType& SelectableGroup::GetContent() const
{
    return objects;
}

inline SelectableGroup::CollectionType& SelectableGroup::GetMutableContent()
{
    return objects;
}

inline bool SelectableGroup::IsEmpty() const
{
    return objects.empty();
}

inline uint32 SelectableGroup::GetSize() const
{
    return static_cast<uint32>(objects.size());
}

inline const AABBox3& SelectableGroup::GetIntegralBoundingBox() const
{
    return integralBoundingBox;
}

template <typename T>
inline SelectableGroup::Enumerator<T> SelectableGroup::ObjectsOfType()
{
    return SelectableGroup::Enumerator<T>(this, objects);
}

template <typename T>
inline SelectableGroup::ConstEnumerator<T> SelectableGroup::ObjectsOfType() const
{
    return SelectableGroup::ConstEnumerator<T>(this, objects);
}

template <typename T>
bool SelectableGroup::ContainsObjectsOfType() const
{
    return std::any_of(objects.begin(), objects.end(), [](const Selectable& obj) { return obj.CanBeCastedTo<T>(); });
}

template <typename T>
bool SelectableGroup::ContainsOnlyObjectsOfType() const
{
    return std::all_of(objects.begin(), objects.end(), [](const Selectable& obj) { return obj.CanBeCastedTo<T>(); });
}

/*
 * Enumerator
 */
template <typename T>
inline SelectableGroup::Enumerator<T>::Enumerator(SelectableGroup* g, SelectableGroup::CollectionType& c)
    : group(g)
    , iBegin(c)
    , iEnd(c, c.size())
{
    group->Lock();
}

template <typename T>
inline SelectableGroup::Enumerator<T>::~Enumerator()
{
    group->Unlock();
}

template <typename T>
inline typename SelectableGroup::Enumerator<T>::Iterator& SelectableGroup::Enumerator<T>::begin()
{
    return iBegin;
}

template <typename T>
inline typename SelectableGroup::Enumerator<T>::Iterator& SelectableGroup::Enumerator<T>::end()
{
    return iEnd;
}

/*
 * Iterator
 */
template <typename T>
inline SelectableGroup::Enumerator<T>::Iterator::Iterator(SelectableGroup::CollectionType& c)
    : endIndex(c.size())
    , collection(c)
{
    while ((index < endIndex) && (collection[index].template CanBeCastedTo<T>() == false))
    {
        ++index;
    }
}

template <typename T>
inline SelectableGroup::Enumerator<T>::Iterator::Iterator(SelectableGroup::CollectionType& c, size_type end)
    : index(end)
    , endIndex(c.size())
    , collection(c)
{
}

template <typename T>
inline typename SelectableGroup::Enumerator<T>::Iterator& SelectableGroup::Enumerator<T>::Iterator::operator++()
{
    for (++index; index < endIndex; ++index)
    {
        if (collection[index].template CanBeCastedTo<T>())
            break;
    }
    return *this;
}

template <typename T>
inline bool SelectableGroup::Enumerator<T>::Iterator::operator!=(const typename SelectableGroup::Enumerator<T>::Iterator& other) const
{
    return index != other.index;
}

template <typename T>
inline T* SelectableGroup::Enumerator<T>::Iterator::operator*()
{
    DVASSERT(collection[index].template CanBeCastedTo<T>());
    return collection[index].template Cast<T>();
}

/*
 * Const Enumerator
 */
template <typename T>
inline SelectableGroup::ConstEnumerator<T>::ConstEnumerator(const SelectableGroup* g, const SelectableGroup::CollectionType& c)
    : group(g)
    , iBegin(c)
    , iEnd(c, c.size())
{
}

template <typename T>
inline SelectableGroup::ConstEnumerator<T>::~ConstEnumerator()
{
}

template <typename T>
inline const typename SelectableGroup::ConstEnumerator<T>::Iterator& SelectableGroup::ConstEnumerator<T>::begin() const
{
    return iBegin;
}

template <typename T>
inline const typename SelectableGroup::ConstEnumerator<T>::Iterator& SelectableGroup::ConstEnumerator<T>::end() const
{
    return iEnd;
}

/*
 * Const Iterator
 */
template <typename T>
inline SelectableGroup::ConstEnumerator<T>::Iterator::Iterator(const SelectableGroup::CollectionType& c)
    : endIndex(c.size())
    , collection(c)
{
    while ((index < endIndex) && (collection[index].template CanBeCastedTo<T>() == false))
    {
        ++index;
    }
}

template <typename T>
inline SelectableGroup::ConstEnumerator<T>::Iterator::Iterator(const SelectableGroup::CollectionType& c, size_type end)
    : index(end)
    , endIndex(c.size())
    , collection(c)
{
}

template <typename T>
inline typename SelectableGroup::ConstEnumerator<T>::Iterator& SelectableGroup::ConstEnumerator<T>::Iterator::operator++()
{
    for (++index; index < endIndex; ++index)
    {
        if (collection[index].template CanBeCastedTo<T>())
            break;
    }
    return *this;
}

template <typename T>
inline bool SelectableGroup::ConstEnumerator<T>::Iterator::operator!=(const typename SelectableGroup::ConstEnumerator<T>::Iterator& other) const
{
    return index != other.index;
}

template <typename T>
inline T* SelectableGroup::ConstEnumerator<T>::Iterator::operator*() const
{
    DVASSERT(collection[index].template CanBeCastedTo<T>());
    return collection[index].template Cast<T>();
}
} // namespace DAVA
