//
// C++ Implementation: trackcollection
//
// Description: 
//
//
// Author: Tue Haste Andersen <haste@diku.dk>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "trackcollection.h"
#include "xmlparse.h"
#include "trackinfoobject.h"
#include <qfileinfo.h>
#include "defs.h"


TrackCollection::TrackCollection()
{
    m_iCounter = 0;
}

TrackCollection::~TrackCollection()
{
}

void TrackCollection::readXML(QDomNode node)
{
    // For each track...
    QDomNode tracknode = node.firstChild();
    while (!tracknode.isNull())
    {
        if (tracknode.isElement() && tracknode.nodeName()=="Track")
        {
            TrackInfoObject *pTrack = new TrackInfoObject(tracknode);
            addTrack(pTrack);

            // Update counter
            if (pTrack->getId()>m_iCounter)
                m_iCounter = pTrack->getId();
        }
        tracknode = tracknode.nextSibling();
    }
}

void TrackCollection::writeXML(QDomDocument &domXML, QDomElement &root)
{
    qDebug("ELEMENTS %i",m_qTrackList.count());
    QDomElement trackroot = domXML.createElement("TrackList");

    QPtrList<TrackInfoObject>::iterator it = m_qTrackList.begin();
    while (it!=m_qTrackList.end())
    {
        QDomElement elementNew = domXML.createElement("Track");
        (*it)->writeToXML(domXML, elementNew);
        trackroot.appendChild(elementNew);

        ++it;
    }
    root.appendChild(trackroot);
}

void TrackCollection::addTrack(TrackInfoObject *pTrack)
{
    // If id is not already set in the TrackInfoObject, assign it an ID,
    // and increase the ID counter.
    if (pTrack->getId()<=0)
    {
        m_iCounter++;
        pTrack->setId(m_iCounter);
    }
    m_qTrackList.append(pTrack);

}

TrackInfoObject *TrackCollection::getTrack(int id)
{
    // Search through list to find the track of the given id
    QPtrList<TrackInfoObject>::iterator it = m_qTrackList.begin();
    while (it!=m_qTrackList.end())
    {
        if ((*it)->getId()==id)
            break;
        ++it;
    }

    if (it && (*it)->getId()==id)
        return (*it);
    else
        return 0;
}

TrackInfoObject *TrackCollection::getTrack(QString location)
{
    // Search through list to find the track of the given filename
    QPtrList<TrackInfoObject>::iterator it = m_qTrackList.begin();
    while (it!=m_qTrackList.end())
    {
        if ((*it)->getLocation()==location)
            break;
        ++it;
    }
qDebug("not in collection");
    if (it && (*it)->getLocation()==location)
        return (*it);
    else
	{
        // We didn't find the track in the collection, so add a new entry
        QFileInfo file(location);
        if (file.exists())
        {
qDebug("exists");
            TrackInfoObject *pTrack = new TrackInfoObject(file.dirPath(), file.fileName());
qDebug("new TIO");
            // Add track to the collection
            if (pTrack->parse() == OK)
            {
qDebug("parse ok");
                addTrack(pTrack);
                qDebug("Found new track: %s", pTrack->getFilename().latin1());
                return pTrack;
            }
            else
            {
                qWarning("Could not parse %s", file.fileName().latin1());
                delete pTrack;
            }
        }
    }
    return 0;
}
