from setuptools import setup

package_name = 'gait_recognition'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='hemingshan',
    maintainer_email='hemingshan_1999@163.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'Network = gait_recognition.Network:main',
            'lstm_method = gait_recognition.lstm_method',
            'Gait_Recognition = gait_recognition.Gait_Recognition:main'

        ],
    },
)
