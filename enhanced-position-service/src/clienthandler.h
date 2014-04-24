/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup EnhancedPositionService
*
* \copyright Copyright (C) BMW Car IT GmbH 2011, 2012
* 
* \license
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
* this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* @licence end@
**************************************************************************/

#ifndef CLIENTHANDLER_H_
#define CLIENTHANDLER_H_

const QString params[] = {
    "Name",
    "MaxUpdateRate",
    "MinUpdateRate",
    "RequiresDeadReckoning",
    ""
};

class PositionAgent {
private:
    QString m_agentPath;
    QString m_agentUniqueName;
    QString m_paramName;

public:
    PositionAgent(const QString& path, const QString& uniqueName) :
        m_agentPath(""), m_agentUniqueName(""), m_paramName(uniqueName) {
        m_agentPath = path;
        m_agentUniqueName = uniqueName;
        qDebug("k-tor: %s, %s", m_agentPath.toUtf8().data(),
                m_agentUniqueName.toUtf8().data());
    }

    QString toString() const {
        QString temp;
        QTextStream ts(&temp);
        ts << "Agent{" << m_paramName << ", "
                << m_agentUniqueName << ", "
                << m_agentPath << "}";
        return temp;
    }

    QString getPath() const {
        return m_agentPath;
    }

    QString getUniqueName() const {
        return m_agentUniqueName;
    }

    // some helper for printing a list of Strings
    QString listToString(const QStringList& list) const {
        QString res;
        QTextStream ts(&res);

        ts << "[";

        QStringListIterator i(list);
        while (i.hasNext()) {
            const QString& s = i.next();
            ts << s.toUtf8().constData();
            if (i.hasNext())
                ts << ", ";
        }
        ts << "]";

        return res;
    }

    bool handleConfigParameter(const QString& key, const QString& value) {
        if (key.size() == 0 || value.size() == 0)
            return false;

        //
        // some fix values
        if (key.compare("Name") == 0) {
            qDebug("    ++ agent with name '%s'", value.toUtf8().constData());
            m_paramName = value;
            return true;
        }

        // TODO: more flexible ...
        int i;
        QStringList candidates;
        for (i = 0; params[i].size() != 0; ++i) {
            if (params[i].compare(key) == 0) {
                return true;
            }

            // check for similarities - miss typed?
            if (checkForSimilarParameter(params[i], key)) {
                candidates.append(params[i]);
            }
        }

        if (candidates.size() > 0) {
            qDebug("*    found candidate for invalid parameter:\n*    do you mean %s instead of %s",
                    listToString(candidates).toUtf8().constData(),
                    key.toUtf8().constData());
        }

        return false;
    }

    static bool checkForSimilarParameter(const QString& shouldStr,
            const QString& wrongStr) {
        if (shouldStr.isEmpty() || wrongStr.isEmpty())
            return false;

        //qDebug("  compare '%s' with '%s'",
        //        sollString.toUtf8().constData(),
        //        falschString.toUtf8().constData());

        // split 'AbcDef' into 'Abc', 'Def'
        QRegExp rx("([A-Z]+[a-z]*)");
        QStringList parts;
        int pos = 0;
        while ((pos = rx.indexIn(wrongStr, pos)) != -1) {
            parts << rx.cap(1);
            pos += rx.matchedLength();
        }

        // find strings in shouldStr pattern
        QStringList::const_iterator i;
        QString word;
        for (i = parts.begin(); i != parts.end(); ++i) {
            word = *i;
            if (shouldStr.contains(word)) {
                return true;
            }
        }

        return false;
    }

    bool parseAgentParameters(QList<QString> params) {
        bool configError = false;

        // handle configuration parameters. only printed, no other usage.
        if (params.isEmpty()) {
            qDebug("  - no configuration parameters given");
            return false;
        }

        QStringList::const_iterator i;
        for (i = params.begin(); i != params.end(); ++i) {
            const QString& param = *i;
            QStringList values = param.split('=');

            if (values.size() > 1 && handleConfigParameter(values.at(0),
                    values.at(1))) {
                qDebug("  - '%s' = '%s'", values[0].toUtf8().constData(),
                        values[1].toUtf8().constData());
            } else {
                qDebug("  - '%s' (error)", param.toUtf8().constData());
                configError = true;
            }
        }
        return (configError == false);
    }
};

class ClientHandler {
public:
    ClientHandler() {
    }

    bool addNewAgent(QString uniqueName, QString path) {

        if (uniqueName.isEmpty() || path.isEmpty()) {
            qWarning("addNewAgent failed");
            return false;
        }

        PositionAgent agent(path, uniqueName);
        if (agent.getPath().isEmpty() || agent.getUniqueName().isEmpty()) {
            qWarning("failed to crate PositonAgent object");
            return false;
        }

        m_agents.insert(uniqueName, agent);
        printAllAgents();
        return true;
    }

    void printAllAgents() {
        qDebug("******* %d ***********", m_agents.size());
        // Improtant! own copy of list required for iteration!!
        QList<PositionAgent> list = m_agents.values();
        QList<PositionAgent>::const_iterator i;

        for (i = list.begin(); i != list.end(); ++i) {
            PositionAgent s = *i;
            qDebug("*   + %s", s.toString().toUtf8().data());
        }
    }

    bool removeAgent(QString uniqueName, const QString path="") {
        return (m_agents.remove(uniqueName) > 0);
    }

    QList<PositionAgent> values() {
        return m_agents.values();
    }

    QList<QString> keys() {
        return m_agents.keys();
    }

    PositionAgent& getAgent(const QString &key) {
        return *m_agents.find(key);
    }
private:
    QMap<QString, PositionAgent> m_agents;
};

#endif /* CLIENTHANDLER_H_ */
