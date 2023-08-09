#include "appsettings.hpp"

#include <QDirIterator>

static const QString levels_path = "levels/";
static const QString pics_path   = "pics/";


static std::array<unsigned int, (int)appSettings::eTexturesID::End> lTextures;

appSettings::appSettings()
{
}
//------------------------------------------------------------------------------
appSettings::~appSettings()
{
    save();
}
//------------------------------------------------------------------------------
appSettings &appSettings::instance()
{
    static appSettings res;
    return res;
}
//------------------------------------------------------------------------------
void appSettings::load()
{

    mAvailableLevels.clear();

    QDirIterator it(levels_path);

    while (it.hasNext())
    {
        it.next();
        QFileInfo info(it.fileInfo());
        if(info.completeSuffix() == "lvl")
            mAvailableLevels.push_back({info.baseName().toInt(), true});
    }

    std::sort(mAvailableLevels.begin(), mAvailableLevels.end(),
              [](const auto &a, const auto &b) -> bool
              {
                  return a.first < b.first;
              });

    _load_textures();
}
//------------------------------------------------------------------------------
void appSettings::save()
{
}
//------------------------------------------------------------------------------
QString  appSettings::levelsPath() const
{
    return levels_path;
}
//------------------------------------------------------------------------------
unsigned appSettings::screenWidth() const
{
    static unsigned res{800};
    return res;
}
//------------------------------------------------------------------------------
unsigned appSettings::screenHeight() const
{
    static unsigned res{600};
    return res;
}
//------------------------------------------------------------------------------
const std::vector<std::pair<int, bool>> &appSettings::availablesLevels()
{
    return mAvailableLevels;
}
//------------------------------------------------------------------------------
void appSettings::setQGLWidget(QGLWidget &aQGLWidget)
{
    mpGLContext = &aQGLWidget;
}
//------------------------------------------------------------------------------
void appSettings::_load_textures()
{
    glEnable(GL_TEXTURE_2D);

    auto load_lexture = [this](QString aPath) -> unsigned int
    {
        QImage mPngImg;
        mPngImg.load(aPath);
        return mpGLContext->bindTexture(mPngImg);
    };

    lTextures[(int)eTexturesID::Box]    = load_lexture(pics_path + "box_empty.png");
    lTextures[(int)eTexturesID::BoxGoal]= load_lexture(pics_path + "box_goal.png");
    lTextures[(int)eTexturesID::Player] = load_lexture(pics_path + "robot.png");
    lTextures[(int)eTexturesID::Wall]   = load_lexture(pics_path + "wall.png");
    lTextures[(int)eTexturesID::Goal]   = load_lexture(pics_path + "goal.png");

    glDisable(GL_TEXTURE_2D);
}
//------------------------------------------------------------------------------
unsigned int appSettings::textureID(eTexturesID aId)
{
    return lTextures[(int)aId];
}
