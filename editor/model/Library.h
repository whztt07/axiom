#pragma once

#include <QtCore/QString>
#include <QtCore/QUuid>
#include <map>
#include <vector>
#include <functional>

#include "common/Event.h"

namespace AxiomModel {

    class LibraryEntry;

    class Project;

    class Library : public AxiomCommon::Hookable {
    public:
        enum class ConflictResolution {
            KEEP_OLD,
            KEEP_NEW,
            KEEP_BOTH
        };

        AxiomCommon::Event<LibraryEntry *> entryAdded;
        AxiomCommon::Event<const QString &> activeTagChanged;
        AxiomCommon::Event<const QString &> tagAdded;
        AxiomCommon::Event<const QString &> tagRemoved;

        Library();

        Library(Project *project, QDataStream &stream);

        ~Library() override;

        void serialize(QDataStream &stream);

        void import(Library *library, const std::function<ConflictResolution(LibraryEntry *, LibraryEntry *)> &resolveConflict);

        const QString &activeTag() const { return _activeTag; }

        void setActiveTag(const QString &activeTag);

        std::vector<LibraryEntry *> entries() const;

        QStringList tags() const;

        void addEntry(std::unique_ptr<LibraryEntry> entry);

        LibraryEntry *findById(const QUuid &id);

    private:

        std::vector<std::unique_ptr<LibraryEntry>> _entries;
        std::map<QString, size_t> _tags;
        QString _activeTag;

        void addTag(const QString &tag);

        void removeTag(const QString &tag);

        void removeEntry(LibraryEntry *entry);
    };

}