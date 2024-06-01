#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

const int windowWidth = 600;
const int windowHeight = 800;
const int blockSize = 60;
const int initialBlockCount = 5;

sf::Color getRandomColor() {
    return sf::Color(rand() % 256, rand() % 256, rand() % 256);
}

// HSV���� RGB�� ��ȯ�ϴ� �Լ�
sf::Color HSVtoRGB(float h, float s, float v) {
    int hi = static_cast<int>(std::floor(h / 60.0f)) % 6;
    float f = h / 60.0f - std::floor(h / 60.0f);
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch (hi) {
    case 0: return sf::Color(v * 255, t * 255, p * 255);
    case 1: return sf::Color(q * 255, v * 255, p * 255);
    case 2: return sf::Color(p * 255, v * 255, t * 255);
    case 3: return sf::Color(p * 255, q * 255, v * 255);
    case 4: return sf::Color(t * 255, p * 255, v * 255);
    case 5: return sf::Color(v * 255, p * 255, q * 255);
    default: return sf::Color(255, 255, 255); // �� ���� ������ �⺻������ ����
    }
}

// �ؽ�Ʈ �ʱ�ȭ �Լ�
sf::Text initializeText(const std::string& str, const sf::Font& font, int size, sf::Color color, float x, float y) {
    sf::Text text;
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
    return text;
}

// ��� ����ŷ ���� �Լ�
void runBlockStackingGame(sf::RenderWindow& window, float hueTop, float saturationTop, float valueTop,
    float hueBottom, float saturationBottom, float valueBottom) {
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // ���� �õ� ����

    sf::RectangleShape block(sf::Vector2f(blockSize, blockSize));
    block.setFillColor(getRandomColor());
    block.setPosition((windowWidth - blockSize) / 2, 0);

    sf::RectangleShape floor(sf::Vector2f(windowWidth, 5));
    floor.setFillColor(sf::Color::Black);
    floor.setPosition(0, windowHeight - 5);

    std::vector<sf::RectangleShape> blocks;

    // �ʱ� ��� 5�� ����
    for (int i = 0; i < initialBlockCount; ++i) {
        sf::RectangleShape initialBlock(sf::Vector2f(blockSize, blockSize));
        initialBlock.setFillColor(getRandomColor());
        initialBlock.setPosition((windowWidth - blockSize) / 2, windowHeight - (i + 1) * blockSize - 5);
        blocks.push_back(initialBlock);
    }

    bool falling = false;
    float blockSpeed = 3.0f;
    float dropSpeed = 10.0f;

    // �� ����
    sf::View view = window.getDefaultView();
    float viewYOffset = 0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Space)
                    falling = true;
            }
        }

        if (!falling)
        {
            // ��� �¿� �̵�
            static bool movingRight = true;
            if (movingRight)
            {
                block.move(blockSpeed, 0);
                if (block.getPosition().x + blockSize >= windowWidth)
                    movingRight = false;
            }
            else
            {
                block.move(-blockSpeed, 0);
                if (block.getPosition().x <= 0)
                    movingRight = true;
            }
        }
        else
        {
            // ��� ������
            block.move(0, dropSpeed);

            // ��� �浹 ���� �� �ױ�
            for (const auto& placedBlock : blocks)
            {
                if (block.getGlobalBounds().intersects(placedBlock.getGlobalBounds()))
                {
                    block.setPosition(block.getPosition().x, placedBlock.getPosition().y - blockSize);
                    blocks.push_back(block);
                    block.setFillColor(getRandomColor()); // �� ��� ���� ����
                    block.setPosition((windowWidth - blockSize) / 2, viewYOffset);
                    falling = false;
                    viewYOffset -= blockSize; // �� ������ ����
                    view.setCenter(windowWidth / 2, windowHeight / 2 + viewYOffset);
                    window.setView(view);
                    break;
                }
            }

            // �ٴڿ� ������ ����
            if (block.getPosition().y + blockSize >= windowHeight + viewYOffset - 5)
            {
                block.setPosition(block.getPosition().x, windowHeight + viewYOffset - blockSize - 5);
                blocks.push_back(block);
                block.setFillColor(getRandomColor()); // �� ��� ���� ����
                block.setPosition((windowWidth - blockSize) / 2, viewYOffset);
                falling = false;
                viewYOffset -= blockSize; // �� ������ ����
                view.setCenter(windowWidth / 2, windowHeight / 2 + viewYOffset);
                window.setView(view);
            }
        }

        window.clear();
        // �׶��̼� ��� �׸���
        float gradientStartY = windowHeight + viewYOffset;
        for (int y = 0; y < windowHeight; ++y) {
            float t = static_cast<float>(y) / windowHeight; // 0.0 ~ 1.0 ���� ��
            float hue = (1.0f - t) * hueTop + t * hueBottom;
            float saturation = (1.0f - t) * saturationTop + t * saturationBottom;
            float value = (1.0f - t) * valueTop + t * valueBottom;

            sf::Color bgColor = HSVtoRGB(hue, saturation, value);

            sf::RectangleShape rect(sf::Vector2f(windowWidth, 1));
            rect.setPosition(0, gradientStartY - y);
            rect.setFillColor(bgColor);
            window.draw(rect);
        }

        for (const auto& placedBlock : blocks)
        {
            window.draw(placedBlock);
        }
        window.draw(block);
        window.draw(floor);
        window.display();
    }
}
int main() {
    // ������ ����
    sf::RenderWindow window(sf::VideoMode(800, 600), "Stack Game");

    // ��Ʈ �ε�
    sf::Font font;
    if (!font.loadFromFile("28 Days Later.ttf")) {
        std::cerr << "ERROR" << std::endl;
        return -1;
    }

    // �ؽ�Ʈ �ʱ�ȭ
    sf::Text title = initializeText("Stack Game", font, 50, sf::Color::White, 250, 100);
    sf::Text startButton = initializeText("START", font, 30, sf::Color::White, 330, 300);
    sf::Text exitButton = initializeText("EXIT", font, 30, sf::Color::White, 350, 400);

    // ���� ���� �� ä��, �� ����
    float hueTop = 0.0f;
    float saturationTop = 0.01f;
    float valueTop = 0.9f;

    // �Ʒ��� ���� �� ä��, �� ����
    float hueBottom = 0.0f;
    float saturationBottom = 0.01f;
    float valueBottom = 0.1f;

    // �ӵ� ����
    float hueSpeed = 0.01f; // ���� ��ȭ �ӵ�
    float saturationSpeed = 0.0001f; // ä�� ��ȭ �ӵ�
    float valueSpeed = 0.00005f;      // �� ��ȭ �ӵ�

    // ���� ����
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::FloatRect startButtonRect = startButton.getGlobalBounds();

                    // "���� ����" ��ư Ŭ�� Ȯ��
                    if (startButtonRect.contains(static_cast<sf::Vector2f>(mousePos))) {
                        std::cout << "���� ���� ��ư Ŭ��!" << std::endl;
                        window.setFramerateLimit(60); // ������ ����Ʈ ���� ����
                        runBlockStackingGame(window, hueTop, saturationTop, valueTop, hueBottom, saturationBottom, valueBottom); // ��� ����ŷ �������� �̵�
                        return 0; // ���� ���� �� ���α׷� ����
                    }

                    // "����" ��ư Ŭ�� Ȯ��
                    sf::FloatRect exitButtonRect = exitButton.getGlobalBounds();
                    if (exitButtonRect.contains(static_cast<sf::Vector2f>(mousePos))) {
                        std::cout << "���� ��ư Ŭ��!" << std::endl;
                        window.close();
                    }
                }
            }
        }

        // ȭ�� �����
        window.clear();

        // ��ܺ��� �ϴܱ��� �׶��̼� �׸���
        for (int y = 0; y < window.getSize().y; ++y) {
            float t = static_cast<float>(y) / window.getSize().y; // 0.0 ~ 1.0 ���� ��
            float hue = (1.0f - t) * hueTop + t * hueBottom;
            float saturation = (1.0f - t) * saturationTop + t * saturationBottom;
            float value = (1.0f - t) * valueTop + t * valueBottom;

            sf::Color bgColor = HSVtoRGB(hue, saturation, value);

            sf::RectangleShape rect(sf::Vector2f(window.getSize().x, 1));
            rect.setPosition(0, y);
            rect.setFillColor(bgColor);
            window.draw(rect);
        }

        // �ؽ�Ʈ �׸���
        window.draw(title);
        window.draw(startButton);
        window.draw(exitButton);

        // �����쿡 �׸� ���� ǥ��
        window.display();

        hueTop += hueSpeed; // ���� �������� �̵�
        if (hueTop >= 360.0f) {
            hueTop -= 360.0f; // 360���� �ʰ��ϸ� �ٽ� 0���� ���ư�
        }
        hueBottom += hueSpeed; // ���� �������� �̵�
        if (hueBottom >= 360.0f) {
            hueBottom -= 360.0f; // 360���� �ʰ��ϸ� �ٽ� 0���� ���ư�
        }
        // ���� ä�� ������Ʈ
        saturationTop += saturationSpeed;
        if (saturationTop >= 1.0f || saturationTop <= 0.0f) {
            saturationSpeed *= -1; // ������ �ٲ�
        }

        valueTop += valueSpeed;
        if (valueTop >= 1.0f || valueTop <= 0.5f) {
            valueSpeed *= -1; // ������ �ٲ�
        }

        saturationBottom += saturationSpeed;
        if (saturationBottom >= 1.0f || saturationBottom <= 0.0f) {
            saturationSpeed *= -1; // ������ �ٲ�
        }

        valueBottom += valueSpeed;
        if (valueBottom >= 1.0f || valueBottom <= 0.1f) {
            valueSpeed *= -1; // ������ �ٲ�
        }
    }

    return 0;
}

