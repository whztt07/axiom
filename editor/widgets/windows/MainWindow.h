#pragma once

#include <QtCore/QFileSystemWatcher>
#include <QtCore/QLockFile>
#include <QtCore/QTimer>
#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
#include <memory>
#include <unordered_map>

#include "WindowResizer.h"
#include "editor/backend/AudioBackend.h"
#include "editor/compiler/interface/Runtime.h"
#include "editor/model/Project.h"

namespace AxiomModel {
    class Project;

    class Library;

    class NodeSurface;
}

namespace ads {
    class CDockManager;
}

namespace AxiomGui {

    class NodeSurfacePanel;

    class HistoryPanel;

    class ModuleBrowserPanel;

    class MainWindow : public QMainWindow, public AxiomCommon::TrackedObject {
        Q_OBJECT

    public:
        QAction fileNewAction;
        QAction fileImportLibraryAction;
        QAction fileExportLibraryAction;
        QAction fileOpenAction;
        QAction fileSaveAction;
        QAction fileSaveAsAction;
        QAction fileExportAction;
        QAction fileQuitAction;

        QAction editUndoAction;
        QAction editRedoAction;
        QAction editCutAction;
        QAction editCopyAction;
        QAction editPasteAction;
        QAction editDeleteAction;
        QAction editSelectAllAction;
        QAction editPreferencesAction;

        QAction helpAboutAction;

        explicit MainWindow(AxiomBackend::AudioBackend *backend);

        ~MainWindow() override;

        MaximCompiler::Runtime *runtime() { return &_runtime; }

        AxiomModel::Project *project() const { return _project.get(); }

        AxiomModel::Library *library() const { return _library.get(); }

        void setProject(std::unique_ptr<AxiomModel::Project> project);

        static QString globalLibraryLockPath();

        static QString globalLibraryFilePath();

        void lockGlobalLibrary();

        void unlockGlobalLibrary();

        void openProjectFrom(const QString &path);

        void setResizerVisible(bool isVisible);

    signals:
        void resized(QSize newSize);

    public slots:

        NodeSurfacePanel *showSurface(NodeSurfacePanel *fromPanel, AxiomModel::NodeSurface *schematic, bool split,
                                      bool permanent);

        void showAbout();

        void newProject();

    protected:
        void keyPressEvent(QKeyEvent *event) override;

        void keyReleaseEvent(QKeyEvent *event) override;

        void closeEvent(QCloseEvent *event) override;

        void dragEnterEvent(QDragEnterEvent *event) override;

        void dropEvent(QDropEvent *event) override;

        void resizeEvent(QResizeEvent *event) override;

    private slots:

        void removeSurface(AxiomModel::NodeSurface *surface);

        void openProject();

        void saveProject();

        void saveAsProject();

        void exportProject();

        void importLibrary();

        void exportLibrary();

        void importLibraryFrom(const QString &path);

    private:
        ads::CDockManager *dockManager;
        AxiomBackend::AudioBackend *_backend;
        MaximCompiler::Runtime _runtime;
        std::unique_ptr<AxiomModel::Project> _project;
        std::unique_ptr<AxiomModel::Library> _library;
        std::unordered_map<AxiomModel::NodeSurface *, std::unique_ptr<NodeSurfacePanel>> _openPanels;
        std::unique_ptr<HistoryPanel> _historyPanel;
        std::unique_ptr<ModuleBrowserPanel> _modulePanel;
        QMenu *_viewMenu;
        QLockFile libraryLock;
        bool isLibraryLocked = false;
        QTimer saveDebounceTimer;
        QTimer loadDebounceTimer;
        QFileSystemWatcher globalLibraryWatcher;

        QSize beforeResizeSize;
        WindowResizer rightResizer;
        WindowResizer bottomResizer;
        WindowResizer bottomRightResizer;

        bool didJustSaveLibrary = false;
        bool isLoadingLibrary = false;

        static std::unique_ptr<AxiomModel::Library> loadGlobalLibrary();
        static std::unique_ptr<AxiomModel::Library> loadDefaultLibrary();

        void saveGlobalLibrary();

        void triggerLibraryChanged();

        void saveProjectTo(const QString &path);

        bool checkCloseProject();

        void updateWindowTitle(const QString &linkedFile, bool isDirty);

        bool isInputFieldFocused() const;

        void updateResizerGeometry();

    private slots:
        void triggerLibraryChangeDebounce();

        void triggerLibraryReload();

        void triggerLibraryReloadDebounce();

        void startedResize();

        void rightResized(QPoint resizeAmount);

        void bottomResized(QPoint resizeAmount);

        void bottomRightResized(QPoint resizeAmount);
    };
}
