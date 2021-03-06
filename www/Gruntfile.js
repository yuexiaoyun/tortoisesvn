'use strict';

module.exports = function(grunt) {

    // Project configuration.
    grunt.initConfig({
        dirs: {
            dest: 'dist',
            src: 'source'
        },
        ver: grunt.file.readJSON('version.json'),

        // Copy files that don't need compilation to dist/
        copy: {
            dist: {
                files: [
                    {dest: '<%= dirs.dest %>/', src: ['*', '!*.html'], filter: 'isFile', expand: true, cwd: '<%= dirs.src %>/'},
                    {dest: '<%= dirs.dest %>/', src: '.htaccess', expand: true, cwd: '<%= dirs.src %>/'},
                    {dest: '<%= dirs.dest %>/', src: 'files/**', expand: true, cwd: '<%= dirs.src %>/'},
                    {dest: '<%= dirs.dest %>/', src: ['img/**', '!**/_old/**'], expand: true, cwd: '<%= dirs.src %>/'},
                    {dest: '<%= dirs.dest %>/', src: ['js/*.min.js', 'js/prettify/**'], expand: true, cwd: '<%= dirs.src %>/'},
                ]
            }
        },

        includereplace: {
            dist: {
                options: {
                    globals: {
                        bottomHtml: '',
                        headHtml: '',
                        metaDescription: '',
                        metaKeywords: '',
                        DATE: '<%= grunt.template.today("dddd, mmmm dS, yyyy, HH:MM:ss Z") %>',
                        TSVNSHORTVERSION: '<%= ver.TSVNSHORTVERSION %>',
                        TSVNVERSION: '<%= ver.TSVNVERSION %>',
                        SVNVERSION: '<%= ver.SVNVERSION %>'
                    }
                },
                files: [
                    {src: '*.html', dest: '<%= dirs.dest %>/', expand: true, cwd: '<%= dirs.src %>/'}
                ]
            }
        },

        concat: {
            prettify: {
                src: '<%= dirs.src %>/css/prettify.css',
                dest: '<%= dirs.dest %>/css/prettify.min.css'
            },
            core: {
                src: ['<%= dirs.src %>/css/normalize.css',
                      '<%= dirs.src %>/css/jquery.fancybox.css',
                      '<%= dirs.src %>/css/style.css'
                ],
                dest: '<%= dirs.dest %>/css/pack.css'
            },
            js: {
                src: ['<%= dirs.src %>/js/jquery.mousewheel.js',
                      '<%= dirs.src %>/js/jquery.fancybox.js'
                ],
                dest: '<%= dirs.dest %>/js/pack.js'
            }
        },

        uncss: {
            options: {
                htmlroot: '<%= dirs.dest %>',
                ignore: [/(#|\.)fancybox(\-[a-zA-Z]+)?/],
                ignoreSheets: [/fonts.googleapis/, /www.google.com/],
                stylesheets: ['css/pack.css']
            },
            dist: {
                src: '<%= dirs.dest %>/**/*.html',
                dest: '<%= concat.core.dest %>'
            }
        },

        cssmin: {
            options: {
                compatibility: 'ie8',
                keepSpecialComments: 0
            },
            prettify: {
                src: '<%= concat.prettify.dest %>',
                dest: '<%= concat.prettify.dest %>'
            },
            core: {
                src: '<%= concat.core.dest %>',
                dest: '<%= concat.core.dest %>'
            }
        },

        uglify: {
            options: {
                compress: {
                    warnings: false
                },
                mangle: true,
                preserveComments: false
            },
            dist: {
                files: {
                    '<%= concat.js.dest %>': '<%= concat.js.dest %>'
                }
            }
        },

        htmlmin: {
            dist: {
                options: {
                    collapseWhitespace: true,
                    minifyCSS: true,
                    minifyJS: true,
                    removeAttributeQuotes: true,
                    removeComments: true
                },
                expand: true,
                cwd: '<%= dirs.dest %>',
                dest: '<%= dirs.dest %>',
                src: ['**/*.html']
            }
        },

        filerev: {
            css: {
                src: '<%= dirs.dest %>/css/**/{,*/}*.css'
             },
            js: {
                src: [
                    '<%= dirs.dest %>/js/**/{,*/}*.js',
                    '!<%= dirs.dest %>/js/jquery*.min.js',
                    '!<%= dirs.dest %>/js/prettify/lang-*.js'
                ]
            },
            images: {
                src: [
                    '<%= dirs.dest %>/img/**/*.{jpg,jpeg,gif,png}'
                ]
            }
        },

        useminPrepare: {
            html: '<%= dirs.dest %>/index.html',
            options: {
                dest: '<%= dirs.dest %>',
                root: '<%= dirs.dest %>'
            }
        },

        usemin: {
            css: '<%= dirs.dest %>/css/pack*.css',
            html: '<%= dirs.dest %>/**/*.html'
        },

        sitemap: {
            dist: {
                pattern: ['<%= dirs.dest %>/**/*.html', '!<%= dirs.dest %>/**/google*.html'],
                siteRoot: './dist'
            }
        },

        connect: {
            options: {
                hostname: 'localhost',
                livereload: 35729,
                port: 8001
            },
            livereload: {
                 options: {
                    base: '<%= dirs.dest %>/',
                    open: true  // Automatically open the webpage in the default browser
                 }
             }
        },

        watch: {
            options: {
                livereload: '<%= connect.options.livereload %>'
            },
            dev: {
            files: ['<%= dirs.src %>/**', '.csslintrc', '.jshintrc', 'Gruntfile.js', 'version.json'],
                tasks: 'dev'
            },
            build: {
            files: ['<%= dirs.src %>/**', '.csslintrc', '.jshintrc', 'Gruntfile.js', 'version.json'],
                tasks: 'build'
            }
        },

        clean: {
            dist: '<%= dirs.dest %>/'
        },

        csslint: {
            options: {
                csslintrc: '.csslintrc'
            },
            src: [
                '<%= dirs.src %>/css/style.css'
            ]
        },

        jshint: {
            options: {
                jshintrc: '.jshintrc'
            },
            grunt: {
                src: 'Gruntfile.js'
            }
        },

        htmllint: {
            src: '<%= dirs.dest %>/**/*.html'
        },

    });

    // Load any grunt plugins found in package.json.
    require('load-grunt-tasks')(grunt, {scope: 'devDependencies'});
    require('time-grunt')(grunt);

    grunt.registerTask('dev', [
        'clean',
        'copy',
        'includereplace',
        'useminPrepare',
        'concat',
        'filerev',
        'usemin',
        'sitemap'
    ]);

    grunt.registerTask('build', [
        'clean',
        'copy',
        'includereplace',
        'sitemap',
        'useminPrepare',
        'concat',
        'uncss',
        'cssmin',
        'uglify',
        'filerev',
        'usemin',
        'htmlmin'
    ]);

    grunt.registerTask('test', [
        'build',
        'csslint',
        'jshint',
        'htmllint'
    ]);

    grunt.registerTask('server', [
        'build',
        'connect',
        'watch:build'
    ]);

    grunt.registerTask('default', [
        'dev',
        'connect',
        'watch:dev'
    ]);

};
