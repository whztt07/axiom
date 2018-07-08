#include "AxiomEditor.h"

#include "AxiomApplication.h"
#include "backend/AudioBackend.h"
#include "model/Project.h"

static std::unique_ptr<AxiomModel::Project> createProject(MaximCompiler::Runtime *runtime,
                                                          AxiomBackend::AudioBackend *backend) {
    auto project = std::make_unique<AxiomModel::Project>(backend->createDefaultConfiguration());

    // note: backend must be attached before runtime, as attachRuntime will trigger the first build and
    // configuration update
    project->mainRoot().attachBackend(backend);
    project->mainRoot().attachRuntime(runtime);

    return std::move(project);
}

AxiomEditor::AxiomEditor(AxiomBackend::AudioBackend *backend)
    : runtime(true, false, AxiomApplication::main.jit()), window(createProject(&runtime, backend)) {}

int AxiomEditor::run() {
    window.show();
    return AxiomApplication::main.exec();
}

void AxiomEditor::show() {
    window.show();
}

void AxiomEditor::hide() {
    window.hide();
}

void AxiomEditor::idle() {
    AxiomApplication::main.processEvents();
    AxiomApplication::main.sendPostedEvents(&window);
}
